#include "Behave.h"

#include "Crystal.h"
#include "CutoutFns.h"
#include "Diffraction.h"
#include "Efficiency.h"
#include "EventType.h"
#include "InvocationState.h"
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
Ray behaveCrystal(Ray r, const Behaviour behaviour, [[maybe_unused]] Collision col) {
    Behaviour::Crystal b = variant::get<Behaviour::Crystal>(behaviour.m_behaviour);

    double theta0    = getTheta(ray.direction, col.normal, b.m_offsetAngle);
    double bragg     = getBraggAngle(ray.energy, b.m_dSpacing2);
    double asymmetry = -getAsymmetryFactor(bragg, b.m_offsetAngle);

    double polFactorS = 1.0;
    double polFactorP = std::fabs(cos(2 * bragg));

    double wavelength = energyToWaveLength(ray.energy);
    double gamma      = getDiffractionPrefactor(wavelength, b.m_unitCellVolume);

    std::complex<double> F0(b.m_structureFactorReF0, b.m_structureFactorImF0);
    std::complex<double> FH(b.m_structureFactorReFH, b.m_structureFactorImFH);
    std::complex<double> FHC(b.m_structureFactorReFHC, b.m_structureFactorImFHC);

    auto etaS = computeEta(theta0, bragg, asymmetry, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC,
                           b.m_structureFactorImFHC, b.m_structureFactorReF0, b.m_structureFactorImF0, polFactorS, gamma);

    auto etaP = computeEta(theta0, bragg, asymmetry, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC,
                           b.m_structureFactorImFHC, b.m_structureFactorReF0, b.m_structureFactorImF0, polFactorP, gamma);

    auto RS = computeR(etaS, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC, b.m_structureFactorImFHC);
    auto RP = computeR(etaP, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC, b.m_structureFactorImFHC);

    const auto incident_vec = ray.direction;
    const auto reflect_vec  = glm::reflect(incident_vec, col.normal);
    ray.direction           = reflect_vec;

    ComplexFresnelCoeffs fresnelCoeff = {RS, RP};

    const auto reflect_field = interceptReflectCrystal(ray.electric_field, incident_vec, reflect_vec, col.normal, fresnelCoeff);
    ray.electric_field       = reflect_field;
    ray.order                = 0;
}

// Implementation based on dynamical diffraction theory from:
// Batterman, B. W., & Cole, H. (1964). "Dynamical Diffraction of X Rays by Perfect Crystals".
// Reviews of Modern Physics, 36(3), 681-717. https://doi.org/10.1103/RevModPhys.36.681
RAYX_FN_ACC
Ray behaveSlit(Ray r, const Behaviour behaviour, Rand& __restrict rand) {
    Behaviour::Slit b = variant::get<Behaviour::Slit>(behaviour.m_behaviour);

    // slit lies in x-y plane instead of x-z plane as other elements
    Cutout openingCutout  = b.m_openingCutout;
    Cutout beamstopCutout = b.m_beamstopCutout;
    bool withinOpening    = inCutout(openingCutout, ray.position.x, ray.position.z);
    bool withinBeamstop   = inCutout(beamstopCutout, ray.position.x, ray.position.z);

    if (!withinOpening || withinBeamstop) {
        terminateRay(ray.event_type, EventType::Absorbed);
        return;
    }

    double phi;
    double psi;
    directionToSphericalCoords(ray.direction, phi, psi);

    double dPhi       = 0;
    double dPsi       = 0;
    double wavelength = energyToWaveLength(ray.energy);

    // this was previously called "diffraction"
    if (wavelength > 0) {
        variant ::visit(
            [&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
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

    sphericalCoordsToDirection(phi, psi, ray.direction);

    ray.order = 0;
}

RAYX_FN_ACC
Ray behaveRZP(Ray r, const Behaviour behaviour, const Collision col, Rand& __restrict rand) {
    Behaviour::RZP b = variant::get<Behaviour::RZP>(behaviour.m_behaviour);

    double WL            = energyToWaveLength(ray.energy);
    double Ord           = b.m_orderOfDiffraction;
    int additional_order = int(b.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(ray.position, col.normal, b, DX, DZ);

    // if additional zero order should be behaved, approx. half of the rays are randomly chosen to be behaved in order 0 (= ordinary reflection)
    // instead of the given order
    if (additional_order == 1) {
        if (ray.rand.randomDouble() > 0.5) Ord = 0;
    }

    // only 2D case, not 2 1D gratings with 90 degree rotation as in old RAY
    double az = WL * DZ * Ord * 1e-6;
    double ax = WL * DX * Ord * 1e-6;
    refrac2D(ray, col.normal, az, ax);

    ray.order = static_cast<int>(Ord);
}

RAYX_FN_ACC
Ray behaveGrating(Ray r, const Behaviour behaviour, const Collision col) {
    Behaviour::Grating b = variant::get<Behaviour::Grating>(behaviour.m_behaviour);

    // vls parameters passed in q.elementParams
    double WL                 = energyToWaveLength(ray.energy);
    double lineDensity        = b.m_lineDensity;
    double orderOfDiffraction = b.m_orderOfDiffraction;

    // adjusted linedensity = WL * default_linedensity * order * 1e-06
    double adjustedLinedensity = vlsGrating(lineDensity, col.normal, ray.position.z, b.m_vls) * WL * orderOfDiffraction * 1e-06;
    ray.order                  = static_cast<int>(orderOfDiffraction);
    // no additional zero order here?

    // refraction
    refrac2D(ray, col.normal, adjustedLinedensity, 0);
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
        OneCoating oneCoating = deserializeOneCoating(coating);

        constexpr int vacuum_material = -1;
        const auto vacuum_ior = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable);
        const auto coating_ior = getRefractiveIndex(r.m_energy, oneCoating.material, materialIndices, materialTable);
        const auto substrate_ior = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

        const auto angle = angleBetweenUnitVectors(-incident_vec, col.normal);
        const auto incidentAngle = complex::Complex(angle == 0.0 ? 1e-8 : angle, 0.0);

        const double wavelength = energyToWaveLength(r.m_energy);

        const auto amplitude = computeSingleCoatingReflectance(
            incidentAngle,
            wavelength,
            oneCoating.thickness,
            vacuum_ior,
            coating_ior,
            substrate_ior
        );

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
Ray behaveFoil(Ray r, const Behaviour behaviour, const Collision col, const int material, const int* __restrict materialIndices,
               const double* __restrict materialTable) {
    Behaviour::Foil f = variant::get<Behaviour::Foil>(behaviour.m_behaviour);
    const double wavelength = energyToWaveLength(r.m_energy);

    const auto indexVacuum   = complex::Complex(1., 0.);
    const auto indexMaterial = getRefractiveIndex(ray.energy, material, materials.indices, materials.tables);

    double angle = angleBetweenUnitVectors(-ray.direction, col.normal);  // in rad

#if !defined(RAYX_CUDA_ENABLED)
    // std::isnan is not tagged as device function, when compiling with nvcc
    using std::isnan;
#endif

    if (isnan(angle) || angle == 0) angle = 1e-8;
    const auto incidentAngle = complex::Complex(angle, 0);

    const auto totalTransmission = computeTransmittance(wavelength, incidentAngle, indexVacuum, indexMaterial, f.m_thicknessSubstrate);

    // calc efficiency
    ray.electric_field = interceptFoil(ray.electric_field, ray.direction, col.normal, totalTransmission);

    ray.order = 0;
}

RAYX_FN_ACC void behaveImagePlane(Ray& __restrict ray) { ray.order = 0; }

RAYX_FN_ACC
void behave(Ray& __restrict ray, const CollisionPoint& __restrict col, const OpticalElement& __restrict element, const Materials materials) {
    switch (element.m_behaviour.m_type) {
        case BehaveType::ImagePlane:
            break;
        case BehaveType::Mirror:
            behaveMirror(ray, col, element.m_material, materials);
            break;
        case BehaveType::Grating:
            behaveGrating(ray, element.m_behaviour, col);
            break;
        case BehaveType::Slit:
            behaveSlit(ray, element.m_behaviour);
            break;
        case BehaveType::RZP:
            behaveRZP(ray, element.m_behaviour, col);
            break;
        case BehaveType::Crystal:
            behaveCrystal(ray, element.m_behaviour, col);
            break;
        case BehaveType::Foil:
            behaveFoil(ray, element.m_behaviour, col, element.m_material, materials);
            break;
    }
}

}  // namespace RAYX
