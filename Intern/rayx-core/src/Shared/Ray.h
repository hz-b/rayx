#ifndef RAY_H
#define RAY_H

#include "adapt.h"

#ifndef GLSL
namespace RAYX {
#endif

// Note: A `dvec3` needs an alignment of 4 * sizeof(double), hence two dvec3s can never be directly after each other (without padding).
// Further, the number of doubles in a Ray need to be divisible by four at all times, as we want to store multiple Rays after each other without
// padding in `rayData`. This is why we need m_extraParam.
struct RAYX_API Ray {
    dvec3 m_position;
    // The m_eventType is only required for Rays stored as part of a RayHistory or BundleHistory. It then expresses the type of event that occured at
    // that point in time. For normal rays during tracing the eventType will be undefined. See the potential values of `m_eventType` in `Constants.h`.
    double m_eventType;
    dvec3 m_direction;
    double m_energy;
    dvec4 m_stokes;
    double m_pathLength;
    double m_order;
    double m_lastElement;
    double m_extraParam;
};

#ifndef GLSL
}  // namespace RAYX
#endif

#endif
