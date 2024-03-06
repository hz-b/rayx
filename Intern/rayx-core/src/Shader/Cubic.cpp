#include "Cubic.h"
#include "Approx.h"

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FUNC
dvec3 RAYX_API cubicPosition(Ray r, double alpha) {
    
    double yy = r.m_position[1];
    double y = yy * r8_cos(alpha) - r.m_position[2] * r8_sin(alpha);
    double z = (r.m_position[2]) * r8_cos(alpha) + yy * r8_sin(alpha);

    return dvec3(0, y, z);
}

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
RAYX_FUNC
dvec3 RAYX_API cubicDirection(Ray r, double alpha) {
    
    double am = r.m_direction[1];
    double dy = r.m_direction[1] * r8_cos(alpha) - r.m_direction[2] * r8_sin(alpha);
    double dz = r.m_direction[2] * r8_cos(alpha) + am * r8_sin(alpha);

    return dvec3(0, dy, dz);
}
