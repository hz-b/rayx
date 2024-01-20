#ifndef RAY_H
#define RAY_H

#include "Adapt.h"

#ifndef GLSL
namespace RAYX {
#endif

/// This type represents a photon ray at a fixed point in time.
/// Often we consider Rays at particular points in time - when *something happens to them*.
/// For example, they could currently be colliding with some OpticalElement.
/// These kind of *events* are encoded using the m_eventType field of the Ray.
/// In EventType.h you can see what each eventType value means.

struct RAYX_API Ray {
    /// The position of the ray.
    /// NOTE: The coordinate system in which the position is to be interpreted, depends on m_eventType!
    dvec3 m_position;

    /// The m_eventType expresses what is currently happening to the ray.
    /// During tracing the eventType will be uninitialized (ETYPE_UNINIT).
    /// Only when an event will be recorded, the m_eventType will be set accordingly.
    /// See the potential values of `m_eventType` in `EventType.h`.
    double m_eventType;

    /// The direction of the ray.
    /// The direction is normalized, so its L2 norm (aka length) is one.
    /// NOTE: The coordinate system in which direction is to be interpreted, depends on m_eventType!
    dvec3 m_direction;

    /// The energy of this photon (in eV).
    double m_energy;

    /// The stokes vector, see https://en.wikipedia.org/wiki/Stokes_parameters
    /// They express the polarization of the Ray.
    dvec4 m_stokes;

    /// The distance that this ray has already traveled (in mm).
    double m_pathLength;

    /// The order of diffraction.
    /// Is currently only set to a non-zero value by the RZP.
    double m_order;

    /// the index of the last Element that this ray collided/interacted with.
    /// is initially set to -1 upon construction by the LightSources.
    double m_lastElement;

    /// The index of the LightSource that emitted this ray.
    /// It is initially set to -1 upon construction by the LightSources and later set by the beamline.
    /// NOTE: This can be moved outside of the Ray struct. It is here for convenience.
    double m_sourceID;

    // NOTE: if you intend to mutate the Ray struct, you have to
    // 1. check that `formatAsVec` from Debug.h correctly uses your Ray struct.
    // (This is eg. used for CHECK_EQ and hence very important for the test suite).
    // 2. check that the format defined in Writer.h uses your Ray struct correctly.
    // 3. check that the CSV-parser (within CSVWriter.cpp) correctly reconstructs rays.
    // 4. check whether alignment requirements are still satisfied (should be done by the static_assert below).
};

// Note: A `dvec3` needs an alignment of 4 * sizeof(double), hence two dvec3s can never be directly after each other (without padding).
// Further, the number of doubles in a Ray need to be divisible by four at all times, as we want to store multiple Rays after each other without
// padding in `rayData`. This is why we need m_padding.
#ifndef GLSL
static_assert(sizeof(Ray) % alignof(dvec3) == 0);
#endif

#ifndef GLSL
}  // namespace RAYX
#endif

#endif
