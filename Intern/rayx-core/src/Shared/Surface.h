#ifndef SURFACE_H
#define SURFACE_H

#include "adapt.h"

// a surface is a potentially infinite curved surface in 3d space.
// as our elements are mostly finite in size, they are represented by a (potentially infinite) surface in combination with a finite cutout (see CTYPE
// constants)
const int STYPE_QUADRIC = 0;
const int STYPE_TOROID = 1;
const int STYPE_PLANE_XY = 2;  // an infinite X-Y plane.

struct Surface {
    double m_type;
    double m_params[16];
};

///////////////////
// Quadric
///////////////////

struct QuadricSurface {
    int m_icurv;
    double m_a11;
    double m_a12;
    double m_a13;
    double m_a14;
    double m_a22;
    double m_a23;
    double m_a24;
    double m_a33;
    double m_a34;
    double m_a44;
};

INLINE Surface serializeQuadric(QuadricSurface surface) {
    Surface ser;
    ser.m_type = STYPE_QUADRIC;
    ser.m_params[0] = double(surface.m_icurv);
    ser.m_params[1] = surface.m_a11;
    ser.m_params[2] = surface.m_a12;
    ser.m_params[3] = surface.m_a13;
    ser.m_params[4] = surface.m_a14;
    ser.m_params[5] = surface.m_a22;
    ser.m_params[6] = surface.m_a23;
    ser.m_params[7] = surface.m_a24;
    ser.m_params[8] = surface.m_a33;
    ser.m_params[9] = surface.m_a34;
    ser.m_params[10] = surface.m_a44;
    return ser;
}

INLINE QuadricSurface deserializeQuadric(Surface ser) {
    QuadricSurface surface;
    surface.m_icurv = int(ser.m_params[0]);
    surface.m_a11 = ser.m_params[1];
    surface.m_a12 = ser.m_params[2];
    surface.m_a13 = ser.m_params[3];
    surface.m_a14 = ser.m_params[4];
    surface.m_a22 = ser.m_params[5];
    surface.m_a23 = ser.m_params[6];
    surface.m_a24 = ser.m_params[7];
    surface.m_a33 = ser.m_params[8];
    surface.m_a34 = ser.m_params[9];
    surface.m_a44 = ser.m_params[10];
    return surface;
}

//////////////
// Toroid
/////////////

#define ToroidType double
const ToroidType TOROID_TYPE_CONVEX = 0;
const ToroidType TOROID_TYPE_CONCAVE = 1;

struct ToroidSurface {
    double m_longRadius;
    double m_shortRadius;
    ToroidType m_toroidType;
};

INLINE Surface serializeToroid(ToroidSurface surface) {
    Surface ser;
    ser.m_type = STYPE_TOROID;
    ser.m_params[0] = surface.m_longRadius;
    ser.m_params[1] = surface.m_shortRadius;
    ser.m_params[2] = surface.m_toroidType;
    return ser;
}

INLINE ToroidSurface deserializeToroid(Surface ser) {
    ToroidSurface surface;
    surface.m_longRadius = ser.m_params[0];
    surface.m_shortRadius = ser.m_params[1];
    surface.m_toroidType = ser.m_params[2];
    return surface;
}

/////////////
// Plane XY
/////////////

// `PlaneXY` doesn't have any data so it doesn't need a struct.

INLINE Surface serializePlaneXY() {
    Surface ser;
    ser.m_type = STYPE_PLANE_XY;
    return ser;
}

#endif
