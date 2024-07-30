#include "Cubic.h"

namespace RAYX {

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FN_ACC
glm::dvec3 RAYX_API cubicPosition(Ray r, double alpha) {
    double yy = r.m_position[1];
    double y = yy * glm::cos(alpha) - r.m_position[2] * glm::sin(alpha);
    double z = (r.m_position[2]) * glm::cos(alpha) + yy * glm::sin(alpha);

    return glm::dvec3(0, y, z);
}

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FN_ACC
glm::dvec3 RAYX_API cubicDirection(Ray r, double alpha) {
    double am = r.m_direction[1];
    double dy = r.m_direction[1] * glm::cos(alpha) - r.m_direction[2] * glm::sin(alpha);
    double dz = r.m_direction[2] * glm::cos(alpha) + am * glm::sin(alpha);

    return glm::dvec3(0, dy, dz);
}

}  // namespace RAYX
