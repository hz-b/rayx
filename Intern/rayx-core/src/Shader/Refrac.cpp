#include "Refrac.h"
#include "Helper.h"
#include "EventType.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
RAYX_FN_ACC
Ray refrac2D(Ray r, dvec3 normal, double az, double ax, InvState& inv) {
    double eps1 = -glm::atan(normal.x / normal.y);
    double del1 = glm::asin(normal.z);

    double cos_d = glm::cos(-del1);
    double sin_d = glm::sin(-del1);
    double cos_e = glm::cos(-eps1);
    double sin_e = glm::sin(-eps1);
    dmat3 rot = dmat3(cos_e, cos_d * sin_e, sin_d * sin_e, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, -sin_d, cos_d);
    dmat3 inv_rot = dmat3(cos_e, -sin_e, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, sin_d * sin_e, sin_d * cos_e, cos_d);
    r.m_direction = rot * r.m_direction;

    double x1 = r.m_direction.x - ax;
    double z1 = r.m_direction.z - az;
    double y1 = 1 - x1 * x1 - z1 * z1;

    if (y1 > 0) {
        y1 = sqrt(y1);

        r.m_direction.x = x1;
        r.m_direction.y = y1;
        r.m_direction.z = z1;
        r.m_direction = inv_rot * r.m_direction;
    } else {  // beyond horizon - when divergence too large
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON, inv);
    }
    return r;
}

// refraction function used for gratings
RAYX_FN_ACC
Ray refrac(Ray r, dvec3 normal, double linedensity, InvState& inv) {
    double xy = normal[0] / normal[1];
    double zy = normal[2] / normal[1];
    double sqq = sqrt(1 + zy * zy + xy * xy);
    double an_x = xy / sqq;
    double an_y = -1.0 / sqq;
    double an_z = zy / sqq;

    double eps1 = glm::atan(an_x / an_y);  //-atan(an_x/an_z) around z, chi
    double del1 = glm::asin(an_z);         // sign(an_z) * glm::atan(sqrt( (an_z*an_z) / (1-an_z*an_z) )); //
                                         // -asin(an_z); // -asin around x, psi
    double cos_d = glm::cos(del1);
    double a1 = linedensity * cos_d;
    double sin_d = glm::sin(-del1);
    double cos_e = glm::cos(-eps1);
    double sin_e = glm::sin(-eps1);
    dmat4 rot = dmat4(cos_e, cos_d * sin_e, sin_d * sin_e, 0, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, 0, -sin_d, cos_d, 0, 0, 0, 0, 1);
    dmat4 inv_rot = dmat4(cos_e, -sin_e, 0, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, 0, sin_d * sin_e, sin_d * cos_e, cos_d, 0, 0, 0, 0, 1);
    r.m_direction = dvec3(rot * dvec4(r.m_direction, 0));

    double y1 = (r.m_direction[1] * r.m_direction[1] + r.m_direction[2] * r.m_direction[2] - ((r.m_direction[2] - a1) * (r.m_direction[2] - a1)));
    if (y1 > 0) {
        y1 = sqrt(y1);
        r.m_direction[1] = y1;
        r.m_direction[2] -= a1;
        r.m_direction = dvec3(inv_rot * dvec4(r.m_direction, 0));
    } else {
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON, inv);
    }
    return r;
}

/*
 * simplified refraction function used for plane gratings
 * normal is always the same (0,1,0) -> no rotation and thus no trigonometric
 * functions necessary
 * @param r			ray
 * @param normal		normal at ray-object intersection (for planes always
 * (0,1,0))
 * @param a			a = WL * D0 * ORD * 1.e-6  with D0: line density (l/mm);
 * WL:wavelength (nm); ORD order of diffraction
 */
RAYX_FN_ACC
Ray RAYX_API refracPlane(Ray r, [[maybe_unused]] dvec3 normal,
                          double a, InvState& inv) {  // TODO fix unused var
    double y1 = r.m_direction[1] * r.m_direction[1] + r.m_direction[2] * r.m_direction[2] - (r.m_direction[2] - a) * (r.m_direction[2] - a);
    if (y1 > 0) {
        y1 = sqrt(y1);
        r.m_direction[1] = y1;
        r.m_direction[2] = r.m_direction[2] - a;
    } else {
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON, inv);
    }
    return r;
}

} // namespace RAYX
