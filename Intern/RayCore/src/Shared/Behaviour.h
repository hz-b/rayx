#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Cutout.h"

#ifdef GLSL
#define INLINE
#else
#define INLINE inline
#endif

// A behaviour decides what happens whenever a ray hits the surface of this element.
// Each behaviour type has its own `trace` function.
const int BTYPE_MIRROR = 0;
const int BTYPE_GRATING = 1;
const int BTYPE_SLIT = 2;
const int BTYPE_RZP = 3;
const int BTYPE_IMAGE_PLANE = 4;

struct Behaviour {
    // the type of this behaviour, see the BTYPE constants.
    // the type describes how the m_params need to be interpreted.
    double m_type;
    double m_params[16];
};

////////////////////
// Mirror
////////////////////

// Mirror holds no data so it doesn't need a struct or a deserialize function.
// TODO should contain Material!
INLINE Behaviour serializeMirror() {
    Behaviour b;
    b.m_type = BTYPE_MIRROR;
    return b;
}

///////////////////
// Slit
///////////////////

struct SlitBehaviour {
    double m_beamstopWidth;
    double m_beamstopHeight;
    Cutout m_gapCutout;
};

INLINE Behaviour serializeSlit(SlitBehaviour s) {
    Behaviour b;
    b.m_type = BTYPE_SLIT;
    b.m_params[0] = s.m_beamstopWidth;
    b.m_params[1] = s.m_beamstopHeight;
    b.m_params[2] = s.m_gapCutout.m_type;
    b.m_params[3] = s.m_gapCutout.m_params[0];
    b.m_params[4] = s.m_gapCutout.m_params[1];
    b.m_params[5] = s.m_gapCutout.m_params[2];
    return b;
}

INLINE SlitBehaviour deserializeSlit(Behaviour b) {
    SlitBehaviour s;
    s.m_beamstopWidth = b.m_params[0];
    s.m_beamstopHeight = b.m_params[1];
    s.m_gapCutout.m_type = b.m_params[2];
    s.m_gapCutout.m_params[0] = b.m_params[3];
    s.m_gapCutout.m_params[1] = b.m_params[4];
    s.m_gapCutout.m_params[2] = b.m_params[5];
    return s;
}

/////////////////
// ImagePlane
////////////////

// ImagePlane holds no data so it doesn't need a struct or a deserialize function.
INLINE Behaviour serializeImagePlane() {
    Behaviour b;
    b.m_type = BTYPE_IMAGE_PLANE;
    return b;
}

#endif
