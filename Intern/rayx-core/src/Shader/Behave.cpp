#include "Behave.h"

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
#include "Utils.h"

namespace RAYX {

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

    r.m_order = Ord;
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
    r.m_order = orderOfDiffraction;
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
Ray behaveFoil(Ray r, const Behaviour behaviour, const Collision col, const int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    //FoilBehaviour f = deserializeFoil(behaviour);
    const auto incidentVec = r.m_direction;
    const auto normal = col.normal;

    constexpr int vacuum_material = -1;

    const auto ior_i = getRefractiveIndex(r.m_energy, vacuum_material, materialIndices, materialTable); // vacuum
    const auto ior_t = getRefractiveIndex(r.m_energy, material, materialIndices, materialTable);        // foil

    // Eintritt: Vakuum -> Folie
    const auto refractIn = refract_dvec3(incidentVec, normal, ior_i.real() / ior_t.real());
    if (glm::length(refractIn) == 0.0) return r; // Totalreflexion

    auto incAngle = angleBetweenUnitVectors(incidentVec, normal);
    // when angleBetweenUnitVectors is nan, the value shall be 10⁻8
    if (std::isnan(incAngle) || incAngle == 0) {incAngle = 1e-8;}

    const auto incidentAngle = complex::Complex(incAngle, 0);
    const auto refractAngleIn = calcRefractAngle(incidentAngle, ior_i, ior_t);
    const auto ampIn = calcRefractAmplitude(incidentAngle, refractAngleIn, ior_i, ior_t);

    const auto entryMatrix = calcPolaririzationMatrix(incidentVec, refractIn, normal, ampIn);
    ElectricField ef1 = entryMatrix * r.m_field;

    // Durchlauf durch die Folie
    //const double cos_theta = glm::dot(-refractIn, normal);
    //const double travelDist = f.m_thicknessSubstrate / std::abs(cos_theta);
    // const glm::dvec3 insidePos = col.hitpoint + refractIn * travelDist;

    // Austritt: Folie -> Vakuum
    const auto exitNormal = -normal;
    const auto refractOut = refract_dvec3(refractIn, exitNormal, ior_t.real() / ior_i.real());
    if (glm::length(refractOut) == 0.0) return r; // Totalreflexion

    incAngle = angleBetweenUnitVectors(refractIn, exitNormal);
    // when angleBetweenUnitVectors is nan, the value shall be 10⁻8
    if (std::isnan(incAngle) || incAngle == 0) {incAngle = 1e-8;}

    const auto exitAngle = complex::Complex(incAngle, 0);
    const auto refractAngleOut = calcRefractAngle(exitAngle, ior_t, ior_i);
    const auto ampOut = calcRefractAmplitude(exitAngle, refractAngleOut, ior_t, ior_i);

    const auto exitMatrix = calcPolaririzationMatrix(refractIn, refractOut, exitNormal, ampOut);
    ElectricField ef2 = exitMatrix * ef1;

    r.m_direction = refractOut;
    r.m_field = ef2;
    //r.m_position = insidePos; // optional, je nach Modell
    return r;
}



RAYX_FN_ACC Ray behaveImagePlane(Ray r) { return r; }

}  // namespace RAYX
