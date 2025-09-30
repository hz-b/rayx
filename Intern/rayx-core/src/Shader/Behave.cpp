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
void behaveCrystal(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col) {
    CrystalBehaviour b = deserializeCrystal(behaviour);

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
void behaveSlit(Ray& __restrict ray, const Behaviour& __restrict behaviour) {
    SlitBehaviour b = deserializeSlit(behaviour);

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
        if (openingCutout.m_type == CutoutType::Rect) {
            RectCutout r = deserializeRect(openingCutout);
            fraun_diff(r.m_width, wavelength, dPhi, ray.rand);
            fraun_diff(r.m_length, wavelength, dPsi, ray.rand);
        } else if (openingCutout.m_type == CutoutType::Elliptical) {
            EllipticalCutout e = deserializeElliptical(openingCutout);
            bessel_diff(e.m_diameter_z, wavelength, dPhi, dPsi, ray.rand);
        } else {
            _throw("encountered Slit with unsupported openingCutout: %d!", static_cast<int>(openingCutout.m_type));
        }
    }

    phi += dPhi;
    psi += dPsi;

    sphericalCoordsToDirection(phi, psi, ray.direction);

    ray.order = 0;
}

RAYX_FN_ACC
void behaveRZP(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col) {
    RZPBehaviour b = deserializeRZP(behaviour);

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
void behaveGrating(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col) {
    GratingBehaviour b = deserializeGrating(behaviour);

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
void behaveMirror(Ray& __restrict ray, const CollisionPoint& __restrict col, const int material, const Materials materials) {
    // calculate the new direction after the reflection
    const auto incident_vec = ray.direction;
    const auto reflect_vec  = glm::reflect(incident_vec, col.normal);
    ray.direction           = reflect_vec;
    ray.order               = 0;

    // 100% efficiency reflection
    if (material == -2) return;

    constexpr int vacuum_material = -1;
    const auto ior_i              = getRefractiveIndex(ray.energy, vacuum_material, materials.indices, materials.tables);
    const auto ior_t              = getRefractiveIndex(ray.energy, material, materials.indices, materials.tables);

    ray.electric_field = interceptReflect(ray.electric_field, incident_vec, reflect_vec, col.normal, ior_i, ior_t);
}

RAYX_FN_ACC
void behaveFoil(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col, const int material,
                const Materials materials) {
    FoilBehaviour f         = deserializeFoil(behaviour);
    const double wavelength = energyToWaveLength(ray.energy);

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
