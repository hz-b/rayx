#include "Behave.h"

#include "CutoutFns.h"
#include "Diffraction.h"
#include "Efficiency.h"
#include "EventType.h"
#include "Helper.h"
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
Ray behaveSlit(Ray r, int id, [[maybe_unused]] Collision col, InvState& inv) {
    SlitBehaviour b = deserializeSlit(inv.elements[id].m_behaviour);

    // slit lies in x-y plane instead of x-z plane as other elements
    Cutout openingCutout = b.m_openingCutout;
    Cutout beamstopCutout = b.m_beamstopCutout;
    bool withinOpening = inCutout(openingCutout, r.m_position.x, r.m_position.z);
    bool withinBeamstop = inCutout(beamstopCutout, r.m_position.x, r.m_position.z);

    if (!withinOpening || withinBeamstop) {
        recordFinalEvent(r, ETYPE_ABSORBED, inv);
        return r;
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
            fraun_diff(r.m_width, wavelength, dPhi, inv);
            fraun_diff(r.m_length, wavelength, dPsi, inv);
        } else if (openingCutout.m_type == CutoutType::Elliptical) {
            EllipticalCutout e = deserializeElliptical(openingCutout);
            bessel_diff(e.m_diameter_z, wavelength, dPhi, dPsi, inv);
        } else {
            _throw("encountered Slit with unsupported openingCutout");
        }
    }

    phi += dPhi;
    psi += dPsi;

    sphericalCoordsToDirection(phi, psi, r.m_direction);

    r.m_order = 0;
    return r;
}

RAYX_FN_ACC
Ray behaveRZP(Ray r, int id, Collision col, InvState& inv) {
    RZPBehaviour b = deserializeRZP(inv.elements[id].m_behaviour);

    double WL = hvlam(r.m_energy);
    double Ord = b.m_orderOfDiffraction;
    int additional_order = int(b.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(r, col.normal, b, DX, DZ);

    // if additional zero order should be behaved, approx. half of the rays are randomly chosen to be behaved in order 0 (= ordinary reflection)
    // instead of the given order
    if (additional_order == 1) {
        if (squaresDoubleRNG(inv.ctr) > 0.5) Ord = 0;
    }

    // only 2D case, not 2 1D gratings with 90 degree rotation as in old RAY
    double az = WL * DZ * Ord * 1e-6;
    double ax = WL * DX * Ord * 1e-6;
    r = refrac2D(r, col.normal, az, ax, inv);

    r.m_order = Ord;
    return r;
}

RAYX_FN_ACC
Ray behaveGrating(Ray r, int id, Collision col, InvState& inv) {
    GratingBehaviour b = deserializeGrating(inv.elements[id].m_behaviour);

    // vls parameters passed in q.elementParams
    double WL = hvlam(r.m_energy);
    double lineDensity = b.m_lineDensity;
    double orderOfDiffraction = b.m_orderOfDiffraction;

    // adjusted linedensity = WL * default_linedensity * order * 1e-06
    double adjustedLinedensity = vlsGrating(lineDensity, col.normal, r.m_position.z, b.m_vls) * WL * orderOfDiffraction * 1e-06;
    r.m_order = orderOfDiffraction;
    // no additional zero order here?

    // refraction
    r = refrac2D(r, col.normal, adjustedLinedensity, 0, inv);

    return r;
}

RAYX_FN_ACC
Ray behaveMirror(Ray r, int id, Collision col, InvState& inv) {
    // calculate the new direction after the reflection
    const auto incident_vec = r.m_direction;
    const auto reflect_vec = glm::reflect(incident_vec, col.normal);
    r.m_direction = reflect_vec;

    int mat = int(inv.elements[id].m_material);
    if (mat != -2) {
        constexpr int vacuum_material = -1;
        const auto ior_i = getRefractiveIndex(r.m_energy, vacuum_material, inv);
        const auto ior_t = getRefractiveIndex(r.m_energy, mat, inv);

        const auto reflect_field = interceptReflect(r.m_field, incident_vec, reflect_vec, col.normal, ior_i, ior_t);

        r.m_field = reflect_field;
        r.m_order = 0;
    }
    return r;
}

RAYX_FN_ACC
Ray behaveImagePlane(Ray r, [[maybe_unused]] int id, [[maybe_unused]] Collision col, [[maybe_unused]] InvState& inv) {
    // doesn't need to do anything.
    return r;
}

}  // namespace RAYX
