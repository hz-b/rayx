#pragma once

#include "Ray.h"
#include "Throw.h"

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
    return r;
}

// returns angle between ray direction and surface normal at intersection point
RAYX_FN_ACC double RAYX_API getIncidenceAngle(Ray r, glm::dvec3 normal);

RAYX_FN_ACC
inline constexpr bool isRayActive(const EventType eventType) {
    return eventType == EventType::Emitted || eventType == EventType::HitElement;
}

RAYX_FN_ACC
[[nodiscard]] inline constexpr Ray terminateRay(Ray r, const EventType eventType) {
    _debug_warn(isRayActive(r.m_eventType), "ray about to be terminated, but ray is already terminated!");
    _debug_assert(!isRayActive(eventType), "ray about to be terminated, but provided event type is not a valid termination event type!");
    r.m_eventType = eventType;
    return r;
}

}  // namespace RAYX
