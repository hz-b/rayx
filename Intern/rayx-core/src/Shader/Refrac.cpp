#include "Refrac.h"
#include "Helper.h"
#include "Approx.h"

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
Ray refrac2D(Ray r, dvec3 normal, double az, double ax) {
    double eps1 = -r8_atan(normal.x / normal.y);
    double del1 = r8_asin(normal.z);

    double cos_d = r8_cos(-del1);
    double sin_d = r8_sin(-del1);
    double cos_e = r8_cos(-eps1);
    double sin_e = r8_sin(-eps1);
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
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON);
    }
    return r;
}

// refraction function used for gratings
Ray refrac(Ray r, dvec3 normal, double linedensity) {
    // Rotation to fit collision normal to element normal (see Wiki)
    double xy = normal[0] / normal[1];
    double zy = normal[2] / normal[1];
    double sqq = sqrt(1 + zy * zy + xy * xy);
    double an_x = xy / sqq;
    double an_y = -1.0 / sqq;
    double an_z = zy / sqq;
    double eps1 = r8_atan(an_x / an_y);  //-atan(an_x/an_z) around z, chi
    double del1 = r8_asin(an_z);         // sign(an_z) * r8_atan(sqrt( (an_z*an_z) / (1-an_z*an_z) )); //
                                         // -asin(an_z); // -asin around x, psi
    double cos_d = r8_cos(del1);
    double sin_d = r8_sin(-del1);
    double cos_e = r8_cos(-eps1);
    double sin_e = r8_sin(-eps1);
    dmat3 rot = dmat3(cos_e, cos_d * sin_e, sin_d * sin_e, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, -sin_d, cos_d);
    dmat3 inv_rot = dmat3(cos_e, -sin_e, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, sin_d * sin_e, sin_d * cos_e, cos_d);
    r.m_direction = dvec3(rot * r.m_direction);  // ! The rotation should not be applied if the normal is (0, 1, 0) but it is applied in RAY-UI so we do it too

    // Refraction
    double a1 = linedensity * cos_d;
    double y1 = (r.m_direction[1] * r.m_direction[1] + r.m_direction[2] * r.m_direction[2] - ((r.m_direction[2] - a1) * (r.m_direction[2] - a1)));
    if (y1 > 0) {
        y1 = sqrt(y1);
        r.m_direction[1] = y1;
        r.m_direction[2] -= a1;
        r.m_direction = dvec3(inv_rot * r.m_direction);
    } else {
           recordFinalEvent(r, ETYPE_BEYOND_HORIZON);
    }
    return r;
}
