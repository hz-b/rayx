#include "Utils.h"
#include "InvocationState.h"
#include "Complex.h"
#include "Approx.h"
#include "Rand.h"
#include "Helper.h"
#include "RefractiveIndex.h"

// converts energy in eV to wavelength in nm
double RAYX_API hvlam(double x) {
    if (x == 0) {
        return 0.0;
    }
    return INV_NM_TO_EVOLT / x;
}

// originally we calculcated 1/0, which might be UB (https://stackoverflow.com/questions/5802351/what-happens-when-you-divide-by-0-in-a-shader).
// `return 1e+308 * 2.0;` correctly returns positive infinity on my machine (Rudi), but we have no guarantee that it always does.
// So instead we return a sufficiently large number to be used like positive infinity.
double infinity() { return 1e+308; }

// multiplies position and direction of ray r with transformation matrix m
// r = dot(m, r)
Ray RAYX_API rayMatrixMult(Ray r, const dmat4 m) {
    r.m_position = dvec3(m * dvec4(r.m_position, 1));
    r.m_direction = dvec3(m * dvec4(r.m_direction, 0));
    return r;
}

// returns angle between ray direction and surface normal at intersection point
double RAYX_API getIncidenceAngle(Ray r, dvec3 normal) {
    normal = normalize(normal);
    double ar = dot(dvec3(normal), r.m_direction);
    // cut to interval [-1,1]
    if (ar < -1.0)
        ar = -1.0;
    else if (ar > 1.0)
        ar = 1.0;

    double theta = PI - r8_acos(ar);
    int gt = int(theta > PI / 2);
    // theta in [0, pi]
    theta = gt * (PI - theta) + (1 - gt) * theta;
    return theta;
}

