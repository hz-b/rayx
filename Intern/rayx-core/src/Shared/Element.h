#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "Behaviour.h"
#include "Cutout.h"
#include "Surface.h"
#include "adapt.h"

struct SlopeError {
    float m_sag;  // aka `slopeErrorX`
    float m_mer;  // aka `slopeErrorZ`
    float m_thermalDistortionAmp;
    float m_thermalDistortionSigmaX;
    float m_thermalDistortionSigmaZ;
    float m_cylindricalBowingAmp;
    float m_cylindricalBowingRadius;
};

struct Element {
    // for alignment reasons, the dmat4s are at the beginning of the struct.
    mat4 m_inTrans;
    mat4 m_outTrans;

    Behaviour m_behaviour;

    Surface m_surface;
    Cutout m_cutout;

    // general object information
    SlopeError m_slopeError;
    float m_azimuthalAngle;
    float m_material;

    // This field is unused, it's only there to guarantee that sizeof(Element) is divisible by sizeof(mat4).
    // Should guarantee that std430 in GLSL and c++ have the same memory layout for `Element`.
    float m_padding[1];
};

#endif
