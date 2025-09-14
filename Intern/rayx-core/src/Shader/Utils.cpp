#include "Utils.h"

#include "Constants.h"

namespace RAYX {

// returns angle between ray direction and surface normal at intersection point
RAYX_FN_ACC
double RAYX_API getIncidenceAngle(Ray r, glm::dvec3 normal) {
    normal    = glm::normalize(normal);
    double ar = glm::dot(glm::dvec3(normal), r.m_direction);
    // cut to interval [-1,1]
    if (ar < -1.0)
        ar = -1.0;
    else if (ar > 1.0)
        ar = 1.0;

    double theta = PI - glm::acos(ar);
    int gt       = int(theta > PI / 2);
    // theta in [0, pi]
    theta = gt * (PI - theta) + (1 - gt) * theta;
    return theta;
}

}  // namespace RAYX
