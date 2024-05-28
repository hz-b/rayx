#include "Complex.h"
#include "Constants.h"
#include "Approx.h"

namespace RAYX {

RAYX_FUNC
dvec2 cx_sqrt(dvec2 a) {
    double r     = length(a);
    double rpart = sqrt(0.5 * (r + a.x));
    double ipart = sqrt(0.5 * (r - a.x));
    if (a.y < 0.0) ipart = -ipart;
    return dvec2(rpart, ipart);
}

RAYX_FUNC
dvec2 cx_tan(dvec2 a) { return cx_div(cx_sin(a), cx_cos(a)); }

RAYX_FUNC
dvec2 cx_log(dvec2 a) {
    double rpart = sqrt((a.x * a.x) + (a.y * a.y));
    double ipart = glm::atan(a.y, a.x);
    if (ipart > PI) ipart = ipart - (2.0 * PI);
    return dvec2(glm::log(rpart), ipart);
}

RAYX_FUNC
dvec2 cx_mobius(dvec2 a) {
    dvec2 c1 = a - dvec2(1.0, 0.0);
    dvec2 c2 = a + dvec2(1.0, 0.0);
    return cx_div(c1, c2);
}

RAYX_FUNC
dvec2 cx_z_plus_one_over_z(dvec2 a) { return a + cx_div(dvec2(1.0, 0.0), a); }

RAYX_FUNC
dvec2 cx_z_squared_plus_c(dvec2 z, dvec2 c) { return cx_mul(z, z) + c; }

RAYX_FUNC
dvec2 cx_sin_of_one_over_z(dvec2 z) { return cx_sin(cx_div(dvec2(1.0, 0.0), z)); }

RAYX_FUNC
dvec2 cx_to_polar(dvec2 a) {
    double phi = glm::atan(a.y / a.x);
    double r   = length(a);
    return dvec2(r, phi);
}

RAYX_FUNC
dvec2 cx_pow(dvec2 a, int n) {
    double angle = glm::atan(a.y, a.x);
    double r     = length(a);
    double real  = dpow(r, n) * glm::cos(n * angle);
    double im    = dpow(r, n) * glm::sin(n * angle);
    return dvec2(real, im);
}

} // namespace RAYX
