#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Cutout.h"
#include "Adapt.h"

// A behaviour decides what happens whenever a ray hits the surface of this element.
// Each behaviour type has its own `behave` function in `behave.comp`.
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
INLINE Behaviour serializeMirror() {
    Behaviour b;
    b.m_type = BTYPE_MIRROR;
    return b;
}

////////////////
// GRATING
////////////////

struct GratingBehaviour {
    double m_vls[6];
    double m_lineDensity;
    double m_orderOfDiffraction;
};

INLINE Behaviour serializeGrating(GratingBehaviour g) {
    Behaviour b;
    b.m_type = BTYPE_GRATING;

    b.m_params[0] = g.m_vls[0];
    b.m_params[1] = g.m_vls[1];
    b.m_params[2] = g.m_vls[2];
    b.m_params[3] = g.m_vls[3];
    b.m_params[4] = g.m_vls[4];
    b.m_params[5] = g.m_vls[5];
    b.m_params[6] = g.m_lineDensity;
    b.m_params[7] = g.m_orderOfDiffraction;
    return b;
}

INLINE GratingBehaviour deserializeGrating(Behaviour b) {
    GratingBehaviour g;
    g.m_vls[0] = b.m_params[0];
    g.m_vls[1] = b.m_params[1];
    g.m_vls[2] = b.m_params[2];
    g.m_vls[3] = b.m_params[3];
    g.m_vls[4] = b.m_params[4];
    g.m_vls[5] = b.m_params[5];
    g.m_lineDensity = b.m_params[6];
    g.m_orderOfDiffraction = b.m_params[7];
    return g;
}

///////////////////
// Slit
///////////////////

struct SlitBehaviour {
    // The Slit consists of a ray-blocking wall with a small opening inside it.
    // This is the shape (aka cutout) of this small opening.
    // The set of points in the m_openingCutout need to be a subset of the cutout of the whole object.
    Cutout m_openingCutout;

    // Inside this small opening, there might be a beamstop, which absorbs incoming rays.
    // This cutout needs to be a subset of m_openingCutout.
    Cutout m_beamstopCutout;
};

INLINE Behaviour serializeSlit(SlitBehaviour s) {
    Behaviour b;
    b.m_type = BTYPE_SLIT;

    b.m_params[0] = s.m_openingCutout.m_type;
    b.m_params[1] = s.m_openingCutout.m_params[0];
    b.m_params[2] = s.m_openingCutout.m_params[1];
    b.m_params[3] = s.m_openingCutout.m_params[2];

    b.m_params[4] = s.m_beamstopCutout.m_type;
    b.m_params[5] = s.m_beamstopCutout.m_params[0];
    b.m_params[6] = s.m_beamstopCutout.m_params[1];
    b.m_params[7] = s.m_beamstopCutout.m_params[2];
    return b;
}

INLINE SlitBehaviour deserializeSlit(Behaviour b) {
    SlitBehaviour s;

    s.m_openingCutout.m_type = b.m_params[0];
    s.m_openingCutout.m_params[0] = b.m_params[1];
    s.m_openingCutout.m_params[1] = b.m_params[2];
    s.m_openingCutout.m_params[2] = b.m_params[3];

    s.m_beamstopCutout.m_type = b.m_params[4];
    s.m_beamstopCutout.m_params[0] = b.m_params[5];
    s.m_beamstopCutout.m_params[1] = b.m_params[6];
    s.m_beamstopCutout.m_params[2] = b.m_params[7];
    return s;
}

/////////////////
// RZP
////////////////

struct RZPBehaviour {
    double m_imageType;
    double m_rzpType;
    double m_derivationMethod;
    double m_designWavelength;
    double m_designOrderOfDiffraction;
    double m_orderOfDiffraction;
    double m_fresnelZOffset;
    double m_designSagittalEntranceArmLength;
    double m_designSagittalExitArmLength;
    double m_designMeridionalEntranceArmLength;
    double m_designMeridionalExitArmLength;
    double m_designAlphaAngle;
    double m_designBetaAngle;
    double m_additionalOrder;
};

INLINE Behaviour serializeRZP(RZPBehaviour r) {
    Behaviour b;
    b.m_type = BTYPE_RZP;

    b.m_params[0] = r.m_imageType;
    b.m_params[1] = r.m_rzpType;
    b.m_params[2] = r.m_derivationMethod;
    b.m_params[3] = r.m_designWavelength;
    b.m_params[4] = r.m_designOrderOfDiffraction;
    b.m_params[5] = r.m_orderOfDiffraction;
    b.m_params[6] = r.m_fresnelZOffset;
    b.m_params[7] = r.m_designSagittalEntranceArmLength;
    b.m_params[8] = r.m_designSagittalExitArmLength;
    b.m_params[9] = r.m_designMeridionalEntranceArmLength;
    b.m_params[10] = r.m_designMeridionalExitArmLength;
    b.m_params[11] = r.m_designAlphaAngle;
    b.m_params[12] = r.m_designBetaAngle;
    b.m_params[13] = r.m_additionalOrder;
    return b;
}

INLINE RZPBehaviour deserializeRZP(Behaviour b) {
    RZPBehaviour r;
    r.m_imageType = b.m_params[0];
    r.m_rzpType = b.m_params[1];
    r.m_derivationMethod = b.m_params[2];
    r.m_designWavelength = b.m_params[3];
    r.m_designOrderOfDiffraction = b.m_params[4];
    r.m_orderOfDiffraction = b.m_params[5];
    r.m_fresnelZOffset = b.m_params[6];
    r.m_designSagittalEntranceArmLength = b.m_params[7];
    r.m_designSagittalExitArmLength = b.m_params[8];
    r.m_designMeridionalEntranceArmLength = b.m_params[9];
    r.m_designMeridionalExitArmLength = b.m_params[10];
    r.m_designAlphaAngle = b.m_params[11];
    r.m_designBetaAngle = b.m_params[12];
    r.m_additionalOrder = b.m_params[13];
    return r;
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

