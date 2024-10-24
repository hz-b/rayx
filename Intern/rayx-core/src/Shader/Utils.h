#pragma once

#include "Core.h"
#include "Ray.h"

namespace RAYX {

// converts energy in eV to wavelength in nm
RAYX_FN_ACC double RAYX_API hvlam(double x);

RAYX_FN_ACC double infinity();

// multiplies position and direction of ray r with transformation matrix m
// r = dot(m, r)
RAYX_FN_ACC
inline Ray RAYX_API rayMatrixMult(Ray r, const glm::dmat4 m) {
    r.m_position = glm::dvec3(m * glm::dvec4(r.m_position, 1));
    r.m_direction = glm::dvec3(m * glm::dvec4(r.m_direction, 0));
    r.m_field = glm::dmat3(m) * r.m_field;
    return r;
}

// returns angle between ray direction and surface normal at intersection point
RAYX_FN_ACC double RAYX_API getIncidenceAngle(Ray r, glm::dvec3 normal);

}  // namespace RAYX
