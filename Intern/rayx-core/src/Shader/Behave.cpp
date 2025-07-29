#include "Behave.h"

#include "Crystal.h"
#include "CutoutFns.h"
#include "Diffraction.h"
#include "Efficiency.h"
#include "EventType.h"
#include "LineDensity.h"
#include "Rand.h"
#include "Ray.h"
#include "Refrac.h"
#include "RefractiveIndex.h"
#include "SphericalCoords.h"
#include "Throw.h"
#include "Transmission.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
Ray behaveCrystal(Ray r, const Behaviour::Crystal crystal, [[maybe_unused]] Collision col) {
    double theta0 = getTheta(r, col.normal, crystal.m_offsetAngle);
    double bragg = getBraggAngle(r.m_energy, crystal.m_dSpacing2);
    double asymmetry = -getAsymmetryFactor(bragg, crystal.m_offsetAngle);

    double polFactorS = 1.0;
    double polFactorP = std::fabs(cos(2 * bragg));

    double wavelength = energyToWaveLength(r.m_energy);
    double gamma = getDiffractionPrefactor(wavelength, crystal.m_unitCellVolume);

    std::complex<double> F0(crystal.m_structureFactorReF0, crystal.m_structureFactorImF0);
    std::complex<double> FH(crystal.m_structureFactorReFH, crystal.m_structureFactorImFH);
    std::complex<double> FHC(crystal.m_structureFactorReFHC, crystal.m_structureFactorImFHC);

    auto etaS = computeEta(theta0, bragg, asymmetry, crystal.m_structureFactorReFH, crystal.m_structureFactorImFH, crystal.m_structureFactorReFHC,
                           crystal.m_structureFactorImFHC, crystal.m_structureFactorReF0, crystal.m_structureFactorImF0, polFactorS, gamma);

    auto etaP = computeEta(theta0, bragg, asymmetry, crystal.m_structureFactorReFH, crystal.m_structureFactorImFH, crystal.m_structureFactorReFHC,
                           crystal.m_structureFactorImFHC, crystal.m_structureFactorReF0, crystal.m_structureFactorImF0, polFactorP, gamma);

    auto RS =
        computeR(etaS, crystal.m_structureFactorReFH, crystal.m_structureFactorImFH, crystal.m_structureFactorReFHC, crystal.m_structureFactorImFHC);
    auto RP =
        computeR(etaP, crystal.m_structureFactorReFH, crystal.m_structureFactorImFH, crystal.m_structureFactorReFHC, crystal.m_structureFactorImFHC);

    const auto incident_vec = r.m_direction;
    const auto reflect_vec = glm::reflect(incident_vec, col.normal);
    r.m_direction = reflect_vec;

    ComplexFresnelCoeffs fresnelCoeff = {RS, RP};

    const auto reflect_field = interceptReflectCrystal(r.m_field, incident_vec, reflect_vec, col.normal, fresnelCoeff);
    r.m_field = reflect_field;

    return r;
}

// Implementation based on dynamical diffraction theory from:
// Batterman, B. W., & Cole, H. (1964). "Dynamical Diffraction of X Rays by Perfect Crystals".
// Reviews of Modern Physics, 36(3), 681-717. https://doi.org/10.1103/RevModPhys.36.681

RAYX_FN_ACC
Ray behaveSlit(Ray r, const Behaviour::Slit slit, Rand& __restrict rand) {
    // slit lies in x-y plane instead of x-z plane as other elements
    Cutout openingCutout = slit.m_openingCutout;
    Cutout beamstopCutout = slit.m_beamstopCutout;
    bool withinOpening = inCutout(openingCutout, r.m_position.x, r.m_position.z);
    bool withinBeamstop = inCutout(beamstopCutout, r.m_position.x, r.m_position.z);

    if (!withinOpening || withinBeamstop) {
        return terminateRay(r, EventType::Absorbed);
    }

    double phi;
    double psi;
    directionToSphericalCoords(r.m_direction, phi, psi);

    double dPhi = 0;
    double dPsi = 0;
    double wavelength = energyToWaveLength(r.m_energy);

    // this was previously called "diffraction"
    if (wavelength > 0) {
        variant::visit(
            [&]<typename T>(const T& arg) {
                if constexpr (std::is_same_v<T, Cutout::Rect>) {
                    fraun_diff(arg.m_width, wavelength, dPhi, rand);
                    fraun_diff(arg.m_length, wavelength, dPsi, rand);
                } else if constexpr (std::is_same_v<T, Cutout::Elliptical>) {
                    bessel_diff(arg.m_diameter_z, wavelength, dPhi, dPsi, rand);
                } else {
                    _throw("encountered Slit with unsupported openingCutout!");
                }
            },
            openingCutout.m_variant);
    }

    phi += dPhi;
    psi += dPsi;

    sphericalCoordsToDirection(phi, psi, r.m_direction);

    r.m_order = 0;
    return r;
}

RAYX_FN_ACC
Ray behaveRZP(Ray r, const Behaviour::RZP rzp, const Collision col, Rand& __restrict rand) {
    double WL = energyToWaveLength(r.m_energy);
    double Ord = rzp.m_orderOfDiffraction;
    int additional_order = int(rzp.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(r, col.normal, rzp, DX, DZ);

    // if additional zero order should be behaved, approx. half of the rays are randomly chosen to be behaved in order 0 (= ordinary reflection)
    // instead of the given order
    if (additional_order == 1) {
        if (rand.randomDouble() > 0.5) Ord = 0;
    }

    // only 2D case, not 2 1D gratings with 90 degree rotation as in old RAY
    double az = WL * DZ * Ord * 1e-6;
    double ax = WL * DX * Ord * 1e-6;
    r = refrac2D(r, col.normal, az, ax);

    r.m_order = static_cast<Order>(Ord);
    return r;
}

RAYX_FN_ACC
Ray behaveGrating(Ray r, const Behaviour::Grating grating, const Collision col) {
    // vls parameters passed in q.elementParams
    double WL = energyToWaveLength(r.m_energy);
    double lineDensity = grating.m_lineDensity;
    double orderOfDiffraction = grating.m_orderOfDiffraction;

    // adjusted linedensity = WL * default_linedensity * order * 1e-06
    double adjustedLinedensity = vlsGrating(lineDensity, col.normal, r.m_position.z, grating.m_vls) * WL * orderOfDiffraction * 1e-06;
    r.m_order = static_cast<Order>(orderOfDiffraction);
    // no additional zero order here?

    // refraction
    r = refrac2D(r, col.normal, adjustedLinedensity, 0);

    return r;
}

RAYX_FN_ACC
Ray behaveMirror(Ray r, const Collision col, const Coating coating, const int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    // calculate the new direction after the reflection
    const auto incident_vec = r.m_direction;
    const auto reflect_vec = glm::reflect(incident_vec, col.normal);
    r.m_direction = reflect_vec;

    if (coating.m_type == SurfaceCoatingType::SubstrateOnly) {
        if (material != -2) {
        constexpr int vacuum_material = -1;
        const auto vacuum_ior = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable);
        const auto substrate_ior = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

        const auto reflect_field = interceptReflect(r.m_field, incident_vec, reflect_vec, col.normal, vacuum_ior, substrate_ior);

        r.m_field = reflect_field;
        r.m_order = 0;
        }
    } else if (coating.m_type == SurfaceCoatingType::OneCoating) {
        // Extrahiere Coating-Informationen
        OneCoating oneCoating = deserializeOneCoating(coating);

            // Refraktive Indizes
        constexpr int vacuum_material = -1;
        const auto vacuum_ior = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable);
        const auto coating_ior = getRefractiveIndex(r.m_energy, oneCoating.material, materialIndices, materialTable);
        const auto substrate_ior = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

            // Einfallswinkel bestimmen
        const auto angle = angleBetweenUnitVectors(-incident_vec, col.normal);
        const auto incidentAngle = complex::Complex(angle == 0.0 ? 1e-8 : angle, 0.0);

            // Wellenlänge berechnen
        const double wavelength = energyToWaveLength(r.m_energy);

            // Reflexionsamplituden (ohne Rauigkeit)
        const auto amplitude = computeSingleCoatingReflectance(
            incidentAngle,
            wavelength,
            oneCoating.thickness,
            vacuum_ior,
            coating_ior,
            substrate_ior
        );

            // Polarisationsmatrix anwenden
        const auto polmat = calcPolaririzationMatrix(incident_vec, reflect_vec, col.normal, amplitude);
        r.m_field = polmat * r.m_field;
        r.m_order = 0;
    } else if (coating.m_type == SurfaceCoatingType::MultipleCoatings) {
        MultilayerCoating mlCoating = deserializeMultilayer(coating);
                constexpr int vacuum_material = -1;
        const auto vacuum_ior = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable);
        const auto substrate_ior = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

        const int n = mlCoating.numLayers;
        complex::Complex iors[17];
        double thickness[15];

        iors[0] = vacuum_ior;
        for (int i = 0; i < n; ++i) {
            iors[i + 1] = getRefractiveIndex(r.m_energy, mlCoating.layers[i].material, materialIndices, materialTable);
            thickness[i] = mlCoating.layers[i].thickness;
        }
        iors[n + 1] = substrate_ior;

        const auto angle = angleBetweenUnitVectors(-incident_vec, col.normal);
        const auto incidentAngle = complex::Complex(angle == 0.0 ? 1e-8 : angle, 0.0);

        const double wavelength = energyToWaveLength(r.m_energy);

        const auto amplitude = computeMultilayerReflectance(incidentAngle, wavelength, n, thickness, iors);

        const auto polmat = calcPolaririzationMatrix(incident_vec, reflect_vec, col.normal, amplitude);
        r.m_field = polmat * r.m_field;
        r.m_order = 0;
    }

    return r;
}

RAYX_FN_ACC
Ray behaveFoil(Ray r, const Behaviour::Foil foil, const Collision col, const int material, const int* __restrict materialIndices,
               const double* __restrict materialTable) {
    const double wavelength = energyToWaveLength(r.m_energy);

    const auto indexVacuum = complex::Complex(1., 0.);
    const auto indexMaterial = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

    double angle = angleBetweenUnitVectors(-r.m_direction, col.normal);  // in rad

    // std::isnan is not tagged as device function, when compiling with nvcc
#if !defined(RAYX_CUDA_ENABLED)
    using std::isnan;
#endif
    if (isnan(angle) || angle == 0) angle = 1e-8;
    const auto incidentAngle = complex::Complex(angle, 0);

    const auto totalTransmission = computeTransmittance(wavelength, incidentAngle, indexVacuum, indexMaterial, foil.m_thicknessSubstrate);

    // calc efficiency
    r.m_field = interceptFoil(r.m_field, r.m_direction, col.normal, totalTransmission);

    r.m_eventType = EventType::Transmitted;

    return r;
}

RAYX_FN_ACC Ray behaveImagePlane(Ray r) { return r; }

}  // namespace RAYX
