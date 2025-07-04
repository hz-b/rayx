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
#include "Transmission.h"
#include "Throw.h"
#include "Transmission.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
Ray behaveCrystal(Ray r, const Behaviour behaviour, [[maybe_unused]] Collision col) {
    CrystalBehaviour b = deserializeCrystal(behaviour);

    double theta0 = getTheta(r, col.normal, b.m_offsetAngle);
    double bragg = getBraggAngle(r.m_energy, b.m_dSpacing2);
    double asymmetry = -getAsymmetryFactor(bragg, b.m_offsetAngle);

    double polFactorS = 1.0;
    double polFactorP = std::fabs(cos(2 * bragg));

    double wavelength = inm2eV / r.m_energy;
    double gamma = getDiffractionPrefactor(wavelength, b.m_unitCellVolume);

    std::complex<double> F0(b.m_structureFactorReF0, b.m_structureFactorImF0);
    std::complex<double> FH(b.m_structureFactorReFH, b.m_structureFactorImFH);
    std::complex<double> FHC(b.m_structureFactorReFHC, b.m_structureFactorImFHC);

    auto etaS = computeEta(theta0, bragg, asymmetry, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC,
                           b.m_structureFactorImFHC, b.m_structureFactorReF0, b.m_structureFactorImF0, polFactorS, gamma);

    auto etaP = computeEta(theta0, bragg, asymmetry, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC,
                           b.m_structureFactorImFHC, b.m_structureFactorReF0, b.m_structureFactorImF0, polFactorP, gamma);

    auto RS = computeR(etaS, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC, b.m_structureFactorImFHC);
    auto RP = computeR(etaP, b.m_structureFactorReFH, b.m_structureFactorImFH, b.m_structureFactorReFHC, b.m_structureFactorImFHC);

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
Ray behaveSlit(Ray r, const Behaviour behaviour, Rand& __restrict rand) {
    SlitBehaviour b = deserializeSlit(behaviour);

    // slit lies in x-y plane instead of x-z plane as other elements
    Cutout openingCutout = b.m_openingCutout;
    Cutout beamstopCutout = b.m_beamstopCutout;
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
    double wavelength = hvlam(r.m_energy);

    // this was previously called "diffraction"
    if (wavelength > 0) {
        if (openingCutout.m_type == CutoutType::Rect) {
            RectCutout r = deserializeRect(openingCutout);
            fraun_diff(r.m_width, wavelength, dPhi, rand);
            fraun_diff(r.m_length, wavelength, dPsi, rand);
        } else if (openingCutout.m_type == CutoutType::Elliptical) {
            EllipticalCutout e = deserializeElliptical(openingCutout);
            bessel_diff(e.m_diameter_z, wavelength, dPhi, dPsi, rand);
        } else {
            _throw("encountered Slit with unsupported openingCutout: %d!", static_cast<int>(openingCutout.m_type));
        }
    }

    phi += dPhi;
    psi += dPsi;

    sphericalCoordsToDirection(phi, psi, r.m_direction);

    r.m_order = 0;
    return r;
}

RAYX_FN_ACC
Ray behaveRZP(Ray r, const Behaviour behaviour, const Collision col, Rand& __restrict rand) {
    RZPBehaviour b = deserializeRZP(behaviour);

    double WL = hvlam(r.m_energy);
    double Ord = b.m_orderOfDiffraction;
    int additional_order = int(b.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(r, col.normal, b, DX, DZ);

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
Ray behaveGrating(Ray r, const Behaviour behaviour, const Collision col) {
    GratingBehaviour b = deserializeGrating(behaviour);

    // vls parameters passed in q.elementParams
    double WL = hvlam(r.m_energy);
    double lineDensity = b.m_lineDensity;
    double orderOfDiffraction = b.m_orderOfDiffraction;

    // adjusted linedensity = WL * default_linedensity * order * 1e-06
    double adjustedLinedensity = vlsGrating(lineDensity, col.normal, r.m_position.z, b.m_vls) * WL * orderOfDiffraction * 1e-06;
    r.m_order = static_cast<Order>(orderOfDiffraction);
    // no additional zero order here?

    // refraction
    r = refrac2D(r, col.normal, adjustedLinedensity, 0);

    return r;
}

RAYX_FN_ACC
Ray behaveMirror(Ray r, const Collision col, const int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    // calculate the new direction after the reflection
    const auto incident_vec = r.m_direction;
    const auto reflect_vec = glm::reflect(incident_vec, col.normal);
    r.m_direction = reflect_vec;

    if (material != -2) {
        constexpr int vacuum_material = -1;
        const auto ior_i = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable);
        const auto ior_t = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

        const auto reflect_field = interceptReflect(r.m_field, incident_vec, reflect_vec, col.normal, ior_i, ior_t);

        r.m_field = reflect_field;
        r.m_order = 0;
    }

    return r;
}

RAYX_FN_ACC
Ray behaveFoil(Ray r, const Behaviour behaviour, const Collision col, const int material, const int* __restrict materialIndices,
               const double* __restrict materialTable) {
    FoilBehaviour f = deserializeFoil(behaviour);
    const double wavelength = hvlam(r.m_energy);

    const auto indexVacuum = complex::Complex(1., 0.);
    const auto indexMaterial = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);

    double angle = angleBetweenUnitVectors(-r.m_direction, col.normal);  // in rad

    if (std::isnan(angle) || angle == 0) angle = 1e-8;
    const auto incidentAngle = complex::Complex(angle, 0);

    const auto totalTransmission = computeTransmittance(wavelength, incidentAngle, indexVacuum, indexMaterial, f.m_thicknessSubstrate);

    // calc efficiency
    r.m_field = interceptFoil(r.m_field, r.m_direction, col.normal, totalTransmission);

    r.m_eventType = EventType::Transmitted;

    return r;
}

// behave Lens
RAYX_FN_ACC Ray behaveLens(Ray r, const Behaviour behaviour, const Collision col, int material, const int* __restrict materialIndices, const double* __restrict materialTable, Surface surface, const Cutout cutout) {
    LensBehaviour b = deserializeLens(behaviour);

    // Lens specific behavior implementation goes here
    Collision exitcollision = findCollisionInElementCoords(col.hitpoint, r.m_direction, surface, cutout, true);

    r.m_position = exitcollision.hitpoint;
    return r;
}

RAYX_FN_ACC Ray behaveImagePlane(Ray r) { return r; }

}  // namespace RAYX
