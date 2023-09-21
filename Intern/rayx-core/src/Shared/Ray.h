#ifndef RAY_H
#define RAY_H

#include "adapt.h"

#ifndef GLSL
namespace RAYX {
#endif

// Note: A `vec3` needs an alignment of 4 * sizeof(float), hence two dvec3s can never be directly after each other (without padding).
// Further, the number of doubles in a Ray need to be divisible by four at all times, as we want to store multiple Rays after each other without
// padding in `rayData`. This is why we need m_padding.
struct RAYX_API Ray {
    vec3 m_position;
    // The m_eventType is only required for Rays stored as part of a RayHistory or BundleHistory. It then expresses the type of event that occured at
    // that point in time. For normal rays during tracing the eventType will be undefined. See the potential values of `m_eventType` in `Constants.h`.
    float m_eventType;
    vec3 m_direction;
    float m_energy;
    vec4 m_stokes;
    float m_pathLength;
    float m_order;
    float m_lastElement; // created as -1 by light sources.

    // This float only exists, so that sizeof(Ray) is divisble by sizeof(vec4) for alignment reasons.
    // This should never be read or written.
    float m_padding;
};

#ifndef GLSL
}  // namespace RAYX
#endif

#endif
