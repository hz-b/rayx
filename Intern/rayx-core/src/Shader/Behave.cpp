#include "Behave.h"
#include "Utils.h"
#include "Diffraction.h"
#include "Efficiency.h"
#include "Rand.h"
#include "Helper.h"
#include "SphericalCoords.h"
#include "CutoutFns.h"
#include "Refrac.h"
#include "UpdateStokes.h"
#include "LineDensity.h"

Ray behaveSlit(Ray r, int id, [[maybe_unused]] Collision col, Inv& inv) {
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

    double dPhi     = 0;
    double dPsi     = 0;
    double wavelength = hvlam(r.m_energy);

    // this was previously called "diffraction"
    if (wavelength > 0) {
        if (openingCutout.m_type == CTYPE_RECT) {
            RectCutout r = deserializeRect(openingCutout);
            fraun_diff(r.m_width, wavelength, dPhi, inv);
            fraun_diff(r.m_length, wavelength, dPsi, inv);
        } else if (openingCutout.m_type == CTYPE_ELLIPTICAL) {
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

Ray behaveRZP(Ray r, int id, Collision col, Inv& inv) {
    RZPBehaviour b = deserializeRZP(inv.elements[id].m_behaviour);

    double WL            = hvlam(r.m_energy);
    double Ord           = b.m_orderOfDiffraction;
    int additional_order = int(b.m_additionalOrder);

    // calculate the RZP line density for the position of the intersection on the RZP
    double DX, DZ;
    RZPLineDensity(r, col.normal, b, DX, DZ);

    // if additional zero order should be behaved, approx. half of the rays are randomly chosen to be behaved in order 0 (= ordinary reflection) instead
    // of the given order
    if (additional_order == 1) {
        if (squaresDoubleRNG(inv.ctr) > 0.5) Ord = 0;
    }

    // only 2D case, not 2 1D gratings with 90 degree rotation as in old RAY
    double az = WL * DZ * Ord * 1e-6;
    double ax = WL * DX * Ord * 1e-6;
    r         = refrac2D(r, col.normal, az, ax, inv);

    r.m_order = Ord;
    return r;
}

Ray behaveGrating(Ray r, int id, Collision col, Inv& inv) {
    GratingBehaviour b = deserializeGrating(inv.elements[id].m_behaviour);

    // vls parameters passed in q.elementParams
    double WL                 = hvlam(r.m_energy);
    double lineDensity        = b.m_lineDensity;
    double orderOfDiffraction = b.m_orderOfDiffraction;

    // linedensity = WL * default_linedensity * order * 1e-06
    double linedensity = vlsGrating(lineDensity, r.m_position.z, b.m_vls) * WL * orderOfDiffraction * 1e-06;
    r.m_order            = orderOfDiffraction;
    // no additional zero order here?

    // refraction
    r = refrac(r, col.normal, linedensity, inv);

    return r;
}

Ray behaveMirror(Ray r, int id, Collision col, Inv& inv) {
    // calculate intersection point and normal at intersection point
    double azimuthal_angle = inv.elements[id].m_azimuthalAngle;

    // calculate the new direction after the reflection
    r.m_direction = reflect(r.m_direction, col.normal);

    double real_S, real_P, delta;
    double incidence_angle = getIncidenceAngle(r, col.normal);  // getTheta
    int mat = int(inv.elements[id].m_material);
    if (mat != -2) {
        efficiency(r, real_S, real_P, delta, incidence_angle, mat, inv);

        bool absorbed = updateStokes(r, real_S, real_P, delta, azimuthal_angle, inv);
        if (absorbed) {
            recordFinalEvent(r, ETYPE_ABSORBED, inv);
        }
        r.m_order = 0;
    }
    return r;
}

Ray behaveImagePlane(Ray r, [[maybe_unused]] int id, [[maybe_unused]] Collision col, [[maybe_unused]] Inv& inv) {
    // doesn't need to do anything.
    return r;
}
