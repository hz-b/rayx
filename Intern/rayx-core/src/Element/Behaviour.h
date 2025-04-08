#pragma once

#include "Core.h"
#include "Cutout.h"

namespace RAYX {

#undef m_private_serialization_params

// A behaviour decides what happens whenever a ray hits the surface of this element.
// Each behaviour type has its own `behave` function in `Behave.h`.
enum class BehaveType {
    Mirror,
    Grating,
    Slit,
    RZP,
    ImagePlane,
    Crystal,
};

struct Behaviour {
    // the type of this behaviour, see the BTYPE constants.
    // the type describes how the m_private_serialization_params need to be interpreted.
    BehaveType m_type;

    // These params are private. use the serialize & deserialize functions below instead.
    double m_private_serialization_params[16];
};

struct DesignElement;
enum class RZPType { Elliptical, Meriodional };
enum class CentralBeamstop { None, Rectangle, Elliptical };
Behaviour makeBehaviour(const DesignElement& dele);
Behaviour makeGrating(const DesignElement& dele);  //< creates a Grating Behaviour from the parameters given in `dele`.
Behaviour makeSlit(const DesignElement& dele);
Behaviour makeRZPBehaviour(const DesignElement& dele);
Behaviour makeCrystalBehaviour(const DesignElement& dele);

////////////////////
// Mirror
////////////////////

// Mirror holds no data so it doesn't need a struct or a deserialize function.
RAYX_FN_ACC
inline Behaviour serializeMirror() {
    Behaviour b;
    b.m_type = BehaveType::Mirror;
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

RAYX_FN_ACC
inline Behaviour serializeGrating(GratingBehaviour g) {
    Behaviour b;
    b.m_type = BehaveType::Grating;

    b.m_private_serialization_params[0] = g.m_vls[0];
    b.m_private_serialization_params[1] = g.m_vls[1];
    b.m_private_serialization_params[2] = g.m_vls[2];
    b.m_private_serialization_params[3] = g.m_vls[3];
    b.m_private_serialization_params[4] = g.m_vls[4];
    b.m_private_serialization_params[5] = g.m_vls[5];
    b.m_private_serialization_params[6] = g.m_lineDensity;
    b.m_private_serialization_params[7] = g.m_orderOfDiffraction;
    return b;
}

RAYX_FN_ACC
inline GratingBehaviour deserializeGrating(Behaviour b) {
    GratingBehaviour g;
    g.m_vls[0] = b.m_private_serialization_params[0];
    g.m_vls[1] = b.m_private_serialization_params[1];
    g.m_vls[2] = b.m_private_serialization_params[2];
    g.m_vls[3] = b.m_private_serialization_params[3];
    g.m_vls[4] = b.m_private_serialization_params[4];
    g.m_vls[5] = b.m_private_serialization_params[5];
    g.m_lineDensity = b.m_private_serialization_params[6];
    g.m_orderOfDiffraction = b.m_private_serialization_params[7];
    return g;
}

///////////////////
// Slit
///////////////////

struct RAYX_API SlitBehaviour {
    // The Slit consists of a ray-blocking wall with a small opening inside it.
    // This is the shape (aka cutout) of this small opening.
    // The set of points in the m_openingCutout need to be a subset of the cutout of the whole object.
    Cutout m_openingCutout;

    // Inside this small opening, there might be a beamstop, which absorbs incoming rays.
    // This cutout needs to be a subset of m_openingCutout.
    Cutout m_beamstopCutout;
};

RAYX_FN_ACC
inline Behaviour serializeSlit(SlitBehaviour s) {
    Behaviour b;
    b.m_type = BehaveType::Slit;

    b.m_private_serialization_params[0] = static_cast<double>(s.m_openingCutout.m_type);
    b.m_private_serialization_params[1] = s.m_openingCutout.m_private_serialization_params[0];
    b.m_private_serialization_params[2] = s.m_openingCutout.m_private_serialization_params[1];
    b.m_private_serialization_params[3] = s.m_openingCutout.m_private_serialization_params[2];

    b.m_private_serialization_params[4] = static_cast<double>(s.m_beamstopCutout.m_type);
    b.m_private_serialization_params[5] = s.m_beamstopCutout.m_private_serialization_params[0];
    b.m_private_serialization_params[6] = s.m_beamstopCutout.m_private_serialization_params[1];
    b.m_private_serialization_params[7] = s.m_beamstopCutout.m_private_serialization_params[2];
    return b;
}

RAYX_FN_ACC
inline SlitBehaviour deserializeSlit(Behaviour b) {
    SlitBehaviour s;

    s.m_openingCutout.m_type = static_cast<CutoutType>(b.m_private_serialization_params[0]);
    s.m_openingCutout.m_private_serialization_params[0] = b.m_private_serialization_params[1];
    s.m_openingCutout.m_private_serialization_params[1] = b.m_private_serialization_params[2];
    s.m_openingCutout.m_private_serialization_params[2] = b.m_private_serialization_params[3];

    s.m_beamstopCutout.m_type = static_cast<CutoutType>(b.m_private_serialization_params[4]);
    s.m_beamstopCutout.m_private_serialization_params[0] = b.m_private_serialization_params[5];
    s.m_beamstopCutout.m_private_serialization_params[1] = b.m_private_serialization_params[6];
    s.m_beamstopCutout.m_private_serialization_params[2] = b.m_private_serialization_params[7];
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

RAYX_FN_ACC
inline Behaviour serializeRZP(RZPBehaviour r) {
    Behaviour b;
    b.m_type = BehaveType::RZP;

    b.m_private_serialization_params[0] = r.m_imageType;
    b.m_private_serialization_params[1] = r.m_rzpType;
    b.m_private_serialization_params[2] = r.m_derivationMethod;
    b.m_private_serialization_params[3] = r.m_designWavelength;
    b.m_private_serialization_params[4] = r.m_designOrderOfDiffraction;
    b.m_private_serialization_params[5] = r.m_orderOfDiffraction;
    b.m_private_serialization_params[6] = r.m_fresnelZOffset;
    b.m_private_serialization_params[7] = r.m_designSagittalEntranceArmLength;
    b.m_private_serialization_params[8] = r.m_designSagittalExitArmLength;
    b.m_private_serialization_params[9] = r.m_designMeridionalEntranceArmLength;
    b.m_private_serialization_params[10] = r.m_designMeridionalExitArmLength;
    b.m_private_serialization_params[11] = r.m_designAlphaAngle;
    b.m_private_serialization_params[12] = r.m_designBetaAngle;
    b.m_private_serialization_params[13] = r.m_additionalOrder;
    return b;
}

RAYX_FN_ACC
inline RZPBehaviour deserializeRZP(Behaviour b) {
    RZPBehaviour r;
    r.m_imageType = b.m_private_serialization_params[0];
    r.m_rzpType = b.m_private_serialization_params[1];
    r.m_derivationMethod = b.m_private_serialization_params[2];
    r.m_designWavelength = b.m_private_serialization_params[3];
    r.m_designOrderOfDiffraction = b.m_private_serialization_params[4];
    r.m_orderOfDiffraction = b.m_private_serialization_params[5];
    r.m_fresnelZOffset = b.m_private_serialization_params[6];
    r.m_designSagittalEntranceArmLength = b.m_private_serialization_params[7];
    r.m_designSagittalExitArmLength = b.m_private_serialization_params[8];
    r.m_designMeridionalEntranceArmLength = b.m_private_serialization_params[9];
    r.m_designMeridionalExitArmLength = b.m_private_serialization_params[10];
    r.m_designAlphaAngle = b.m_private_serialization_params[11];
    r.m_designBetaAngle = b.m_private_serialization_params[12];
    r.m_additionalOrder = b.m_private_serialization_params[13];
    return r;
}

/////////////////
// ImagePlane
////////////////

// ImagePlane holds no data so it doesn't need a struct or a deserialize function.
RAYX_FN_ACC
inline Behaviour serializeImagePlane() {
    Behaviour b;
    b.m_type = BehaveType::ImagePlane;
    return b;
}



//////////////
// Crystal
//////////////

struct CrystalBehaviour {
    double m_dSpacing;
    double m_unitCellVolume;
    double m_offsetAngle;
    double m_orderOfDiffraction;

    double m_structureFactorReF0;
    double m_structureFactorImF0;
    double m_structureFactorReFH;
    double m_structureFactorImFH;
    double m_structureFactorReFHC;
    double m_structureFactorImFHC;
};

RAYX_FN_ACC
inline Behaviour serializeCrystal(const CrystalBehaviour& c) {
    Behaviour b;
    b.m_type = BTYPE_CRYSTAL;

    b.m_private_serialization_params[0] = c.m_dSpacing;
    b.m_private_serialization_params[1] = c.m_unitCellVolume;
    b.m_private_serialization_params[2] = c.m_orderOfDiffraction;
    b.m_private_serialization_params[3] = c.m_offsetAngle;

    b.m_private_serialization_params[4] = c.m_structureFactorReF0;
    b.m_private_serialization_params[5] = c.m_structureFactorImF0;
    b.m_private_serialization_params[6] = c.m_structureFactorReFH;
    b.m_private_serialization_params[7] = c.m_structureFactorImFH;
    b.m_private_serialization_params[8] = c.m_structureFactorReFHC;
    b.m_private_serialization_params[9] = c.m_structureFactorImFHC;

    return b;
}

RAYX_FN_ACC
inline CrystalBehaviour deserializeCrystal(const Behaviour& b) {
    CrystalBehaviour c;

    c.m_dSpacing = b.m_private_serialization_params[0];
    c.m_unitCellVolume = b.m_private_serialization_params[1];
    c.m_orderOfDiffraction = b.m_private_serialization_params[2];
    c.m_offsetAngle = b.m_private_serialization_params[3];

    c.m_structureFactorReF0 = b.m_private_serialization_params[4];
    c.m_structureFactorImF0 = b.m_private_serialization_params[5];
    c.m_structureFactorReFH = b.m_private_serialization_params[6];
    c.m_structureFactorImFH = b.m_private_serialization_params[7];
    c.m_structureFactorReFHC = b.m_private_serialization_params[8];
    c.m_structureFactorImFHC = b.m_private_serialization_params[9];

    return c;
}




// This prevents m_private_serialization_params from being used outside of this file - making them practically private.
#define m_private_serialization_params "m_private_serialization_params are private! Use the corresponding serialize & deserialize functions instead."

}  // namespace RAYX
