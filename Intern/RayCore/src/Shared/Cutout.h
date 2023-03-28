#ifndef CUTOUT_H
#define CUTOUT_H

#ifdef GLSL
#define INLINE
#else
#define INLINE inline
#endif

// cutout types:
// a subset of points in the 2d plane. used to limited the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with Ray-UI.
const int CTYPE_RECT = 0;  // cutout parameters are (width, 0, length).
const int CTYPE_ELLIPTICAL = 1;
const int CTYPE_TRAPEZOID = 2;
const int CTYPE_UNLIMITED = 3;  // cutout parameters are (0, 0, 0).

// The two dimensions in a cutout are called x1 and x2.
// How they map to the three dimensions of an element depends on the "primary plane".
// typically it's (x1, x2) = (X, Y) or (x1, x2) = (X, Z).

struct Cutout {
    double m_type;
    double m_params[3];
};

///////////////////////////////////
// Rect
///////////////////////////////////

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

// what are the parameters?
struct EllipticalCutout {
    double m_param1;
    double m_param2;
};

INLINE Cutout serializeElliptical(EllipticalCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_ELLIPTICAL;
    ser.m_params[0] = cut.m_param1;
    ser.m_params[1] = cut.m_param2;
    return ser;
}

INLINE EllipticalCutout deserializeElliptical(Cutout ser) {
    EllipticalCutout cut;
    cut.m_param1 = ser.m_params[0];
    cut.m_param2 = ser.m_params[1];
    return cut;
}

////////////////////////
// Trapezoid
////////////////////////

struct TrapezoidCutout {
    double m_param1;
    double m_param2;
    double m_param3;
};

INLINE Cutout serializeTrapezoid(TrapezoidCutout cut) {
    Cutout ser;
    ser.m_type = CTYPE_TRAPEZOID;
    ser.m_params[0] = cut.m_param1;
    ser.m_params[1] = cut.m_param2;
    ser.m_params[2] = cut.m_param3;
    return ser;
}

INLINE TrapezoidCutout deserializeTrapezoid(Cutout ser) {
    TrapezoidCutout cut;
    cut.m_param1 = ser.m_params[0];
    cut.m_param2 = ser.m_params[1];
    cut.m_param3 = ser.m_params[2];
    return cut;
}

/////////////////////////
// Unlimited
/////////////////////////

// Unlimited doesn't have any data so it doesn't need a struct.

INLINE Cutout serializeUnlimited() {
    Cutout ser;
    ser.m_type = CTYPE_UNLIMITED;
    return ser;
}

#endif
