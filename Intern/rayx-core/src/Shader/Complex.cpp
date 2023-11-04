#include "Complex.h"
#include "Approx.h"

dvec2 cx_sqrt(dvec2 a) {
    double r     = length(a);
    double rpart = sqrt(0.5 * (r + a.x));
    double ipart = sqrt(0.5 * (r - a.x));
    if (a.y < 0.0) ipart = -ipart;
    return dvec2(rpart, ipart);
}

dvec2 cx_tan(dvec2 a) { return cx_div(cx_sin(a), cx_cos(a)); }

dvec2 cx_log(dvec2 a) {
    double rpart = sqrt((a.x * a.x) + (a.y * a.y));
    double ipart = r8_atan2(a.y, a.x);
    if (ipart > PI) ipart = ipart - (2.0 * PI);
    return dvec2(r8_log(rpart), ipart);
}

dvec2 cx_mobius(dvec2 a) {
    dvec2 c1 = a - dvec2(1.0, 0.0);
    dvec2 c2 = a + dvec2(1.0, 0.0);
    return cx_div(c1, c2);
}

dvec2 cx_z_plus_one_over_z(dvec2 a) { return a + cx_div(dvec2(1.0, 0.0), a); }

dvec2 cx_z_squared_plus_c(dvec2 z, dvec2 c) { return cx_mul(z, z) + c; }

dvec2 cx_sin_of_one_over_z(dvec2 z) { return cx_sin(cx_div(dvec2(1.0, 0.0), z)); }

dvec2 cx_to_polar(dvec2 a) {
    double phi = r8_atan(a.y / a.x);
    double r   = length(a);
    return dvec2(r, phi);
}

dvec2 cx_pow(dvec2 a, int n) {
    double angle = r8_atan2(a.y, a.x);
    double r     = length(a);
    double real  = dpow(r, n) * r8_cos(n * angle);
    double im    = dpow(r, n) * r8_sin(n * angle);
    return dvec2(real, im);
}
