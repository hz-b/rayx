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
void behaveCrystal(detail::Ray& __restrict ray, const Behaviour::Crystal& __restrict crystal, const CollisionPoint& __restrict col) {
    double theta0    = getTheta(ray.direction, col.normal, crystal.m_offsetAngle);
    double bragg     = getBraggAngle(ray.energy, crystal.m_dSpacing2);
    double asymmetry = -getAsymmetryFactor(bragg, crystal.m_offsetAngle);

    double polFactorS = 1.0;
    double polFactorP = std::fabs(cos(2 * bragg));

    double wavelength = energyToWaveLength(ray.energy);
    double gamma      = getDiffractionPrefactor(wavelength, crystal.m_unitCellVolume);

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
void behaveSlit(detail::Ray& __restrict ray, const Behaviour::Slit& __restrict slit) {
    // slit lies in x-y plane instead of x-z plane as other elements
    Cutout openingCutout  = slit.m_openingCutout;
    Cutout beamstopCutout = slit.m_beamstopCutout;
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
        openingCutout.visit([&]<typename T>(const T& cutout) {
            if constexpr (std::is_same_v<T, Cutout::Rect>) {
                fraun_diff(cutout.m_width, wavelength, dPhi, ray.rand);
                fraun_diff(cutout.m_length, wavelength, dPsi, ray.rand);
            } else if constexpr (std::is_same_v<T, Cutout::Elliptical>) {
                bessel_diff(cutout.m_diameter_z, wavelength, dPhi, dPsi, ray.rand);
            } else {
                _throw("encountered Slit with unsupported openingCutout!");
            }
        });
    }

    phi += dPhi;
    psi += dPsi;

    sphericalCoordsToDirection(phi, psi, ray.direction);

    ray.order = 0;
}

RAYX_FN_ACC
void behaveRZP(detail::Ray& __restrict ray, const Behaviour::RZP& __restrict rzp, const CollisionPoint& __restrict col) {
    double WL            = energyToWaveLength(ray.energy);
    double Ord           = rzp.m_orderOfDiffraction;
    int additional_order = int(rzp.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(ray.position, col.normal, rzp, DX, DZ);

    // if additional zero order should be behaved, approx. half of the rays are randomly chosen to be behaved in order 0 (= ordinary reflection)
    // instead of the given order
    if (additional_order == 1 && ray.rand.randomDouble() > 0.5) Ord = 0;

    // only 2D case, not 2 1D gratings with 90 degree rotation as in old RAY
    double az = WL * DZ * Ord * 1e-6;
    double ax = WL * DX * Ord * 1e-6;
    refrac2D(ray, col.normal, az, ax);

    ray.order = static_cast<int>(Ord);
}

RAYX_FN_ACC
void behaveGrating(detail::Ray& __restrict ray, const Behaviour::Grating& __restrict grating, const CollisionPoint& __restrict col) {
    // vls parameters passed in q.elementParams
    double WL                 = energyToWaveLength(ray.energy);
    double lineDensity        = grating.m_lineDensity;
    double orderOfDiffraction = grating.m_orderOfDiffraction;

    // adjusted linedensity = WL * default_linedensity * order * 1e-06
    double adjustedLinedensity = vlsGrating(lineDensity, col.normal, ray.position.z, grating.m_vls) * WL * orderOfDiffraction * 1e-06;
    ray.order                  = static_cast<int>(orderOfDiffraction);
    // no additional zero order here?

    // refraction
    refrac2D(ray, col.normal, adjustedLinedensity, 0);
}

RAYX_FN_ACC
void behaveMirror(detail::Ray& __restrict ray, const CollisionPoint& __restrict col, const Coating& __restrict coating, const int material,
                  const int* __restrict materialIndices, const double* __restrict materialTable) {
    // calculate the new direction after the reflection
    const auto incident_vec = ray.direction;
    const auto reflect_vec  = glm::reflect(incident_vec, col.normal);
    ray.direction           = reflect_vec;

    if (coating.is<Coating::SubstrateOnly>()) {
        if (material != -2) {
            constexpr int vacuum_material = -1;
            const auto vacuum_ior         = getRefractiveIndex(ray.energy, vacuum_material, materialIndices, materialTable);
            const auto substrate_ior      = getRefractiveIndex(ray.energy, material, materialIndices, materialTable);

            const auto reflect_field = interceptReflect(ray.electric_field, incident_vec, reflect_vec, col.normal, vacuum_ior, substrate_ior);

            ray.electric_field = reflect_field;
            ray.order          = 0;
        }
    } else if (coating.is<Coating::OneCoating>()) {
        Coating::OneCoating oneCoating = coating.get<Coating::OneCoating>();

        constexpr int vacuum_material = -1;
        const auto vacuum_ior         = getRefractiveIndex(ray.energy, vacuum_material, materialIndices, materialTable);
        const auto coating_ior        = getRefractiveIndex(ray.energy, oneCoating.material, materialIndices, materialTable);
        const auto substrate_ior      = getRefractiveIndex(ray.energy, material, materialIndices, materialTable);

        const auto angle         = angleBetweenUnitVectors(-incident_vec, col.normal);
        const auto incidentAngle = complex::Complex(angle == 0.0 ? 1e-8 : angle, 0.0);

        const double wavelength = energyToWaveLength(ray.energy);

        const auto amplitude =
            computeSingleCoatingReflectance(incidentAngle, wavelength, oneCoating.thickness, vacuum_ior, coating_ior, substrate_ior);

        const auto polmat  = calcPolaririzationMatrix(incident_vec, reflect_vec, col.normal, amplitude);
        ray.electric_field = polmat * ray.electric_field;
        ray.order          = 0;
    } else if (coating.is<Coating::MultilayerCoating>()) {
        Coating::MultilayerCoating mlCoating = coating.get<Coating::MultilayerCoating>();
        constexpr int vacuum_material        = -1;
        const auto vacuum_ior                = getRefractiveIndex(ray.energy, vacuum_material, materialIndices, materialTable);
        const auto substrate_ior             = getRefractiveIndex(ray.energy, material, materialIndices, materialTable);

        const int n = mlCoating.numLayers;
        complex::Complex iors[1002];

        iors[0] = vacuum_ior;
        for (int i = 0; i < n; ++i) { iors[i + 1] = getRefractiveIndex(ray.energy, mlCoating.material[i], materialIndices, materialTable); }
        iors[n + 1] = substrate_ior;

        const auto angle         = angleBetweenUnitVectors(-incident_vec, col.normal);
        const auto incidentAngle = complex::Complex(angle == 0.0 ? 1e-8 : angle, 0.0);

        const double wavelength = energyToWaveLength(ray.energy);

        const auto amplitude = computeMultilayerReflectance(incidentAngle, wavelength, n, mlCoating.thickness, iors);

        const auto polmat  = calcPolaririzationMatrix(incident_vec, reflect_vec, col.normal, amplitude);
        ray.electric_field = polmat * ray.electric_field;
        ray.order          = 0;
    } else {
        _throw("encountered Mirror with unsupported Coating type!");
    }
}

RAYX_FN_ACC
void behaveFoil(detail::Ray& __restrict ray, const Behaviour::Foil& __restrict foil, const CollisionPoint& __restrict col, const int material,
                const int* __restrict materialIndices, const double* __restrict materialTable) {
    const auto indexVacuum   = complex::Complex(1., 0.);
    const auto indexMaterial = getRefractiveIndex(ray.energy, material, materialIndices, materialTable);

    double angle = angleBetweenUnitVectors(-ray.direction, col.normal);  // in rad

#if !defined(RAYX_CUDA_ENABLED)
    // std::isnan is not tagged as device function, when compiling with nvcc
    using std::isnan;
#endif

    if (isnan(angle) || angle == 0) angle = 1e-8;
    const auto incidentAngle = complex::Complex(angle, 0);

    const double wavelength = energyToWaveLength(ray.energy);

    const auto totalTransmission = computeTransmittance(wavelength, incidentAngle, indexVacuum, indexMaterial, foil.m_thicknessSubstrate);

    // calc efficiency
    ray.electric_field = interceptFoil(ray.electric_field, ray.direction, col.normal, totalTransmission);

    ray.order = 0;
}

RAYX_FN_ACC void behaveImagePlane(detail::Ray& __restrict ray) { ray.order = 0; }

RAYX_FN_ACC
void behave(detail::Ray& __restrict ray, const CollisionPoint& __restrict col, const OpticalElement& __restrict element,
            const int* __restrict materialIndices, const double* __restrict materialTable) {
    element.m_behaviour.visit([&]<typename T>(const T& behaviour) {
        if constexpr (std::is_same_v<T, Behaviour::Mirror>) {
            behaveMirror(ray, col, element.m_coating, element.m_material, materialIndices, materialTable);
        } else if constexpr (std::is_same_v<T, Behaviour::Grating>) {
            behaveGrating(ray, behaviour, col);
        } else if constexpr (std::is_same_v<T, Behaviour::Slit>) {
            behaveSlit(ray, behaviour);
        } else if constexpr (std::is_same_v<T, Behaviour::RZP>) {
            behaveRZP(ray, behaviour, col);
        } else if constexpr (std::is_same_v<T, Behaviour::Crystal>) {
            behaveCrystal(ray, behaviour, col);
        } else if constexpr (std::is_same_v<T, Behaviour::ImagePlane>) {
            behaveImagePlane(ray);
        } else if constexpr (std::is_same_v<T, Behaviour::Foil>) {
            behaveFoil(ray, behaviour, col, element.m_material, materialIndices, materialTable);
        } else {
            _throw("invalid behaviour type in dynamicElements!");
        }
    });
}

}  // namespace RAYX
