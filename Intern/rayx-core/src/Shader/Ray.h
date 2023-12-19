#ifndef RAY_H
#define RAY_H

#include "Adapt.h"

#ifndef GLSL
namespace RAYX {
#endif

// This type represents a photon ray at a fixed point in time.
// The most frequently used fields are its *position* (in world coordinates) and the *direction* in which it is currently heading.
// Often we consider Rays at particular points in time - when *something happens to them*.
// For example, they could currently be colliding with some OpticalElement.
// These kind of *events* are encoded using the m_eventType field of the Ray.
// In EventType.h you can see what each eventType value means.

// Note: A `dvec3` needs an alignment of 4 * sizeof(double), hence two dvec3s can never be directly after each other (without padding).
// Further, the number of doubles in a Ray need to be divisible by four at all times, as we want to store multiple Rays after each other without
// padding in `rayData`. This is why we need m_padding.
struct RAYX_API Ray {
    dvec3 m_position;
    // The m_eventType is only required for Rays stored as part of a RayHistory or BundleHistory. It then expresses the type of event that occured at
    // that point in time. For normal rays during tracing the eventType will be undefined. See the potential values of `m_eventType` in `EventType.h`.
    double m_eventType;
    dvec3 m_direction;
    double m_energy;
    dvec4 m_stokes;
    double m_pathLength;
    double m_order;
    double m_lastElement; // created as -1 by light sources.

    // This double only exists, so that sizeof(Ray) is divisble by sizeof(dvec4) for alignment reasons.
    // This should never be read or written.
    double m_padding;
};

#ifndef GLSL
}  // namespace RAYX
#endif

#endif
