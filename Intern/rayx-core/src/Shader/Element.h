#pragma once

#include "Common.h"
#include "Behaviour.h"
#include "Cutout.h"
#include "Surface.h"

namespace RAYX {

struct SlopeError {
    double m_sag;  // aka `slopeErrorX`
    double m_mer;  // aka `slopeErrorZ`
    double m_thermalDistortionAmp; // TODO unused
    double m_thermalDistortionSigmaX; // TODO unused
    double m_thermalDistortionSigmaZ; // TODO unused
    double m_cylindricalBowingAmp; // TODO unused
    double m_cylindricalBowingRadius; // TODO unused
};

struct Element {
    // for alignment reasons, the dmat4s are at the beginning of the struct.

    // the "in-transformation": it converts a point from the world coordinates to the element coordinates of this element.
    // The name comes from the fact that an "in-going" ray hitting this elemnet, will first-and-foremost be converted to element coordinates by multiplying with m_inTrans.
    dmat4 m_inTrans;

    // the "out-transformation": it converts a point from element coordinates of this element back to the world coordinates.
    // This is the matrix inverse to m_inTrans.
    dmat4 m_outTrans;

    // The Behaviour expresses what happens to a ray once it collides with this Element.
    Behaviour m_behaviour;

    // The surface expresses how the Elements surface is curved.
    Surface m_surface;

    // The Cutout limits the Surface to the dimensions of the actual Element.
    // Surfaces are often infinite, hence we require the Cutout to limit them.
    Cutout m_cutout;

    // The SlopeError expresses a random noise in the normal vector of a particular surface point on this element.
    // Such a random noise has the (at least visual) effect, that it feels like a rough - uneven - surface.
    // As of today, the `applySlopeError` is the only function using this object.
    SlopeError m_slopeError;

    // The azimuthal angle describes the angle at which this element is rotated around the "main-beam" from the previous element to this element.
    // TODO This is an artifact from RAY-UI, we actually want to get rid of it. (at least it should not be part of the shader model!)
    double m_azimuthalAngle;

    // The material that this object is made of.
    // See the `enum class Material` from Material.h to make sense of this value.
    // Materials are either REFLECTIVE, VACUUM or they represent a particular element from the periodic system.
    double m_material;

    // This field is unused, it's only there to guarantee that sizeof(Element) is divisible by sizeof(dmat4).
    // Should guarantee that std430 in GLSL and c++ have the same memory layout for `Element`.
    double m_padding[1];
};

// make sure Element does not introduce cost on copy or default construction
static_assert(std::is_trivially_copyable_v<Element>);

} // namespace RAYX
