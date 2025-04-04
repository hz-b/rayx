#pragma once

#include <glm.hpp>

#include "Core.h"
#include "ElectricField.h"
#include "EventType.h"

namespace RAYX {

/// This type represents a photon ray at a fixed point in time.
/// Often we consider Rays at particular points in time - when *something happens to them*.
/// For example, they could currently be colliding with some OpticalElement.
/// These kind of *events* are encoded using the m_eventType field of the Ray.
/// In EventType.h you can see what each eventType value means.

struct RAYX_API Ray {
    /// The position of the ray.
    /// NOTE: The coordinate system in which the position is to be interpreted, depends on m_eventType!
    glm::dvec3 m_position;

    /// The m_eventType expresses the kind of event of the previous event
    /// Rays created may be initialized with ETYPE_EMITTED
    /// When an intercept occours, the variable will be set accordingly, either to ETYPE_JUST_HIT_ELEM or to an error type.
    /// See the potential values of `m_eventType` in `EventType.h`.
    double m_eventType;

    /// The direction of the ray.
    /// The direction is normalized, so its L2 norm (aka length) is one.
    /// NOTE: The coordinate system in which direction is to be interpreted, depends on m_eventType!
    glm::dvec3 m_direction;

    /// The energy of this photon (in eV).
    double m_energy;

    /// The complex electric field
    ElectricField m_field;

    /// The distance that this ray has already traveled (in mm).
    double m_pathLength;

    /// The order of diffraction.
    /// Is currently only set to a non-zero value by the RZP.
    double m_order;

    /// the index of the last OpticalElement that this ray collided/interacted with.
    /// is initially set to -1 upon construction by the LightSources.
    double m_lastElement;

    /// The index of the LightSource that emitted this ray.
    /// It is initially set to -1 upon construction by the LightSources and later set by the beamline.
    /// NOTE: This can be moved outside of the Ray struct. It is here for convenience.
    double m_sourceID;

    // NOTE: if you intend to mutate the Ray struct, you have to
    // 1. check that `formatAsVec` from Debug.h correctly uses your Ray struct.
    // 2. check that `checkEq` from setupTests.h works correctly with your struct.
    // 3. check that formatAsVec and checkEq have the SAME ORDER of fields!
    // (checkEq is the backbone of CHECK_EQ, and it will give you wrong outputs, if this order is incorrect.)
    // 4. check that the format defined in Writer.h uses your Ray struct correctly.
    // 5. check that the CSV-parser (within CSVWriter.cpp) correctly reconstructs rays.
    // 6. check that the order in `plot.py` is consistent with the Writer.
    // 7. check whether alignment requirements are still satisfied (should be done by the static_assert below).
};

// make sure Ray does not introduce cost on copy or default construction
static_assert(std::is_trivially_copyable_v<Ray>);

}  // namespace RAYX
