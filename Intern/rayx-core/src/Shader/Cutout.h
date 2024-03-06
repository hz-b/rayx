#ifndef CUTOUT_H
#define CUTOUT_H

#include "Common.h"

#undef m_private_serialization_params

// Cutout types:
// a subset of points of the XZ-plane. used to limit the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with `geometricalShape` from Ray-UI.
const int CTYPE_RECT = 0;
const int CTYPE_ELLIPTICAL = 1;
const int CTYPE_TRAPEZOID = 2;
const int CTYPE_UNLIMITED = 3;

struct Cutout {
    // This types is one of the `CTYPE` constants.
    // It expresses what kind of Cutout this represents.
    double m_type;

    // Parameters that hold information about the cutout.
    // What they mean depends on `m_type`.
    // These parameters shouldn't be accessed manually, use the `serialize` and `deserialize` functions below instead.
    double m_private_serialization_params[3];
};

///////////////////////////////////
// Rect
///////////////////////////////////

// A rectangle specified by width/length centered at (x=0, z=0).
struct RectCutout {
    double m_width; // in X direction
    double m_length; // in Z direction
};

RAYX_FUNC_INLINE
Cutout serializeRect(RectCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_RECT;
    ser.m_private_serialization_params[0] = cut.m_width;
    ser.m_private_serialization_params[1] = cut.m_length;
    return ser;
}

RAYX_FUNC_INLINE
RectCutout deserializeRect(Cutout ser) {
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
struct EllipticalCutout {
    double m_diameter_x;
    double m_diameter_z;
};

RAYX_FUNC
inline Cutout serializeElliptical(EllipticalCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_ELLIPTICAL;
    ser.m_private_serialization_params[0] = cut.m_diameter_x;
    ser.m_private_serialization_params[1] = cut.m_diameter_z;
    return ser;
}

RAYX_FUNC
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
struct TrapezoidCutout {
    double m_widthA; // in X direction
    double m_widthB; // in X direction
    double m_length; // in Z direction
};

RAYX_FUNC
inline Cutout serializeTrapezoid(TrapezoidCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_TRAPEZOID;
    ser.m_private_serialization_params[0] = cut.m_widthA;
    ser.m_private_serialization_params[1] = cut.m_widthB;
    ser.m_private_serialization_params[2] = cut.m_length;
    return ser;
}

RAYX_FUNC
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

RAYX_FUNC
inline Cutout serializeUnlimited() {
    Cutout ser;
    ser.m_type = CTYPE_UNLIMITED;
    return ser;
}

// This prevents m_private_serialization_params from being used outside of this file - making them practically private.
#define m_private_serialization_params "m_private_serialization_params are private! Use the corresponding serialize & deserialize functions instead."


#endif
