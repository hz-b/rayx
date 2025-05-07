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

RAYX_FN_ACC Ray behaveImagePlane(Ray r) { return r; }

}  // namespace RAYX
