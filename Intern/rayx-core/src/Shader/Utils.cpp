#include "Utils.h"

#include "Constants.h"

namespace RAYX {

RAYX_FN_ACC
double RAYX_API energyToWaveLength(double x) { return INV_NM_TO_EVOLT / x; }

RAYX_FN_ACC
double waveLengthToEnergy(const double waveLength) { return INV_NM_TO_EVOLT / waveLength; }

// originally we calculcated 1/0, which might be UB (https://stackoverflow.com/questions/5802351/what-happens-when-you-divide-by-0-in-a-shader).
// `return 1e+308 * 2.0;` correctly returns positive infinity on my machine (Rudi), but we have no guarantee that it always does.
// So instead we return a sufficiently large number to be used like positive infinity.
RAYX_FN_ACC
double infinity() { return 1e+308; }

// returns angle between ray direction and surface normal at intersection point
RAYX_FN_ACC
double RAYX_API getIncidenceAngle(Ray r, glm::dvec3 normal) {
    normal = glm::normalize(normal);
    double ar = glm::dot(glm::dvec3(normal), r.m_direction);
    // cut to interval [-1,1]
    if (ar < -1.0)
        ar = -1.0;
    else if (ar > 1.0)
        ar = 1.0;

    double theta = PI - glm::acos(ar);
    int gt = int(theta > PI / 2);
    // theta in [0, pi]
    theta = gt * (PI - theta) + (1 - gt) * theta;
    return theta;
}

}  // namespace RAYX
