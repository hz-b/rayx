#include "Cubic.h"

namespace RAYX {

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FN_ACC
glm::dvec3 RAYX_API cubicPosition(const glm::dvec3& __restrict rayPosition, double alpha) {
    double yy = rayPosition[1];
    double y  = yy * glm::cos(alpha) - rayPosition[2] * glm::sin(alpha);
    double z  = (rayPosition[2]) * glm::cos(alpha) + yy * glm::sin(alpha);

    return glm::dvec3(0, y, z);
}

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FN_ACC
glm::dvec3 RAYX_API cubicDirection(const glm::dvec3& __restrict rayDirection, double alpha) {
    double am = rayDirection[1];
    double dy = rayDirection[1] * glm::cos(alpha) - rayDirection[2] * glm::sin(alpha);
    double dz = rayDirection[2] * glm::cos(alpha) + am * glm::sin(alpha);

    return glm::dvec3(0, dy, dz);
}

}  // namespace RAYX
