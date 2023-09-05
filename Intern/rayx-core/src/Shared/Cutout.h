#ifndef CUTOUT_H
#define CUTOUT_H

#include "adapt.h"

// Cutout types:
// a subset of points in the 2d plane. used to limited the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with `geometricalShape` from Ray-UI.
const int CTYPE_RECT = 0;
const int CTYPE_ELLIPTICAL = 1;
const int CTYPE_TRAPEZOID = 2;
const int CTYPE_UNLIMITED = 3;

// The two dimensions in a cutout are called x1 and x2.
// How they map to the three dimensions of an element depends on the "primary plane" of its surface.
// typically it's (x1, x2) = (X, Y) or (x1, x2) = (X, Z).

struct Cutout {
    // This types is one of the `CTYPE` constants.
    // It expresses what kind of Cutout this represents.
    double m_type;

    // Parameters that hold information about the cutout.
    // What they mean depends on `m_type`.
    // These parameters shouldn't be accessed manually, use the `serialize` and `deserialize` functions below instead.
    double m_params[3];
};

///////////////////////////////////
// Rect
///////////////////////////////////

// A rectangle with "width" `m_size_x1` and "height" `m_size_x2` centered at (0, 0).
struct RectCutout {
    double m_size_x1;
    double m_size_x2;
};

INLINE Cutout serializeRect(RectCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_RECT;
    ser.m_params[0] = cut.m_size_x1;
    ser.m_params[1] = cut.m_size_x2;
    return ser;
}

INLINE RectCutout deserializeRect(Cutout ser) {
    RectCutout cut;
    cut.m_size_x1 = ser.m_params[0];
    cut.m_size_x2 = ser.m_params[1];
    return cut;
}

////////////////////////////
// Elliptical
////////////////////////////

// https://en.wikipedia.org/wiki/Ellipse
//
// An elliptical shape given by two diameters.
// It can be understood as a circle with individual stretch-factors for both dimensions.
// The point (0, 0) lies at the center of the ellipse.
struct EllipticalCutout {
    double m_diameter_x1;
    double m_diameter_x2;
};

INLINE Cutout serializeElliptical(EllipticalCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_ELLIPTICAL;
    ser.m_params[0] = cut.m_diameter_x1;
    ser.m_params[1] = cut.m_diameter_x2;
    return ser;
}

INLINE EllipticalCutout deserializeElliptical(Cutout ser) {
    EllipticalCutout cut;
    cut.m_diameter_x1 = ser.m_params[0];
    cut.m_diameter_x2 = ser.m_params[1];
    return cut;
}

////////////////////////
// Trapezoid
////////////////////////

// https://en.wikipedia.org/wiki/Trapezoid
//
// A trapezoid consists of two lines with lengths `m_sizeA_x1` and `m_sizeB_x1`, both parallel to the x1 axis.
// These lines have a distance of `m_size_x2`.
// The point (0, 0) lies at the center of the trapezoid.
struct TrapezoidCutout {
    double m_sizeA_x1;
    double m_sizeB_x1;
    double m_size_x2;  // originally called `height`
};

INLINE Cutout serializeTrapezoid(TrapezoidCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_TRAPEZOID;
    ser.m_params[0] = cut.m_sizeA_x1;
    ser.m_params[1] = cut.m_sizeB_x1;
    ser.m_params[2] = cut.m_size_x2;
    return ser;
}

INLINE TrapezoidCutout deserializeTrapezoid(Cutout ser) {
    TrapezoidCutout cut;
    cut.m_sizeA_x1 = ser.m_params[0];
    cut.m_sizeB_x1 = ser.m_params[1];
    cut.m_size_x2 = ser.m_params[2];
    return cut;
}

/////////////////////////
// Unlimited
/////////////////////////

// Every point (x1, x2) is within this cutout, it's unlimited after all.
// `Unlimited` doesn't have any data so it doesn't need a struct.

INLINE Cutout serializeUnlimited() {
    Cutout ser;
    ser.m_type = CTYPE_UNLIMITED;
    return ser;
}

#endif
