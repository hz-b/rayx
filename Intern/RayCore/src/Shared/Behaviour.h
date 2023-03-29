#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

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
