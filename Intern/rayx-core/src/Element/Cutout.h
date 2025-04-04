#pragma once

#include "Core.h"

namespace RAYX {

#undef m_private_serialization_params

// Cutout types:
// a subset of points of the XZ-plane. used to limit the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with `geometricalShape` from Ray-UI.
enum class CutoutType {
    Rect,
    Elliptical,
    Trapezoid,
    Unlimited,
};

struct RAYX_API Cutout {
    // This types is one of the `CTYPE` constants.
    // It expresses what kind of Cutout this represents.
    CutoutType m_type;

    // Parameters that hold information about the cutout.
    // What they mean depends on `m_type`.
    // These parameters shouldn't be accessed manually, use the `serialize` and `deserialize` functions below instead.
    double m_private_serialization_params[3];
};

///////////////////////////////////
// Rect
///////////////////////////////////

// A rectangle specified by width/length centered at (x=0, z=0).
struct RAYX_API RectCutout {
    double m_width;   // in X direction
    double m_length;  // in Z direction
};

RAYX_FN_ACC
inline Cutout serializeRect(RectCutout cut) {
    Cutout ser;
    ser.m_type = CutoutType::Rect;
    ser.m_private_serialization_params[0] = cut.m_width;
    ser.m_private_serialization_params[1] = cut.m_length;
    return ser;
}

RAYX_FN_ACC
inline RectCutout deserializeRect(Cutout ser) {
    RectCutout cut;
    cut.m_width = ser.m_private_serialization_params[0];
    cut.m_length = ser.m_private_serialization_params[1];
    return cut;
}

////////////////////////////
// Elliptical
////////////////////////////

// https://en.wikipedia.org/wiki/Ellipse
//
// An elliptical shape given by two diameters.
// It can be understood as a circle with individual stretch-factors for both dimensions.
// The point (x=0, z=0) lies at the center of the ellipse.
struct RAYX_API EllipticalCutout {
    double m_diameter_x;
    double m_diameter_z;
};

RAYX_FN_ACC
inline Cutout serializeElliptical(EllipticalCutout cut) {
    Cutout ser;
    ser.m_type = CutoutType::Elliptical;
    ser.m_private_serialization_params[0] = cut.m_diameter_x;
    ser.m_private_serialization_params[1] = cut.m_diameter_z;
    return ser;
}

RAYX_FN_ACC
inline EllipticalCutout deserializeElliptical(Cutout ser) {
    EllipticalCutout cut;
    cut.m_diameter_x = ser.m_private_serialization_params[0];
    cut.m_diameter_z = ser.m_private_serialization_params[1];
    return cut;
}

////////////////////////
// Trapezoid
////////////////////////

// https://en.wikipedia.org/wiki/Trapezoid
//
// A trapezoid consists of two lines with lengths `m_widthA` and `m_widthB`, both parallel to the X-axis.
// These lines have a distance of `m_length`.
// The point (x=0, z=0) lies at the center of the trapezoid.
struct RAYX_API TrapezoidCutout {
    double m_widthA;  // in X direction
    double m_widthB;  // in X direction
    double m_length;  // in Z direction
};

RAYX_FN_ACC
inline Cutout serializeTrapezoid(TrapezoidCutout cut) {
    Cutout ser;
    ser.m_type = CutoutType::Trapezoid;
    ser.m_private_serialization_params[0] = cut.m_widthA;
    ser.m_private_serialization_params[1] = cut.m_widthB;
    ser.m_private_serialization_params[2] = cut.m_length;
    return ser;
}

RAYX_FN_ACC
inline TrapezoidCutout deserializeTrapezoid(Cutout ser) {
    TrapezoidCutout cut;
    cut.m_widthA = ser.m_private_serialization_params[0];
    cut.m_widthB = ser.m_private_serialization_params[1];
    cut.m_length = ser.m_private_serialization_params[2];
    return cut;
}

/////////////////////////
// Unlimited
/////////////////////////

// Every point (x, z) is within this cutout, it's unlimited after all.
// `Unlimited` doesn't have any data so it doesn't need a struct.

RAYX_FN_ACC
inline Cutout serializeUnlimited() {
    Cutout ser;
    ser.m_type = CutoutType::Unlimited;
    return ser;
}

// This prevents m_private_serialization_params from being used outside of this file - making them practically private.
#define m_private_serialization_params "m_private_serialization_params are private! Use the corresponding serialize & deserialize functions instead."

}  // namespace RAYX
