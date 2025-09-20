#pragma once

#include <variant>

#include "Core.h"

namespace RAYX {

#undef m_private_serialization_params

// a surface is a potentially infinite curved surface in 3d space.
// as our elements are mostly finite in size, they are represented by a (potentially infinite) surface in combination with a finite cutout (see CTYPE
// constants)
enum class SurfaceType {
    Quadric,
    Toroid,
    Plane,
    Cubic,
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

RAYX_FN_ACC
inline Surface serializeQuadric(QuadricSurface surface) {
    Surface ser;
    ser.m_type = SurfaceType::Quadric;
    ser.m_private_serialization_params[0] = double(surface.m_icurv);
    ser.m_private_serialization_params[1] = surface.m_a11;
    ser.m_private_serialization_params[2] = surface.m_a12;
    ser.m_private_serialization_params[3] = surface.m_a13;
    ser.m_private_serialization_params[4] = surface.m_a14;
    ser.m_private_serialization_params[5] = surface.m_a22;
    ser.m_private_serialization_params[6] = surface.m_a23;
    ser.m_private_serialization_params[7] = surface.m_a24;
    ser.m_private_serialization_params[8] = surface.m_a33;
    ser.m_private_serialization_params[9] = surface.m_a34;
    ser.m_private_serialization_params[10] = surface.m_a44;
    return ser;
}

RAYX_FN_ACC
inline QuadricSurface deserializeQuadric(Surface ser) {
    QuadricSurface surface;
    surface.m_icurv = int(ser.m_private_serialization_params[0]);
    surface.m_a11 = ser.m_private_serialization_params[1];
    surface.m_a12 = ser.m_private_serialization_params[2];
    surface.m_a13 = ser.m_private_serialization_params[3];
    surface.m_a14 = ser.m_private_serialization_params[4];
    surface.m_a22 = ser.m_private_serialization_params[5];
    surface.m_a23 = ser.m_private_serialization_params[6];
    surface.m_a24 = ser.m_private_serialization_params[7];
    surface.m_a33 = ser.m_private_serialization_params[8];
    surface.m_a34 = ser.m_private_serialization_params[9];
    surface.m_a44 = ser.m_private_serialization_params[10];
    return surface;
}

//////////////
// Toroid
/////////////

#define ToroidType double
constexpr ToroidType TOROID_TYPE_CONVEX  = 0;
constexpr ToroidType TOROID_TYPE_CONCAVE = 1;

struct ToroidSurface {
    double m_longRadius;
    double m_shortRadius;
    ToroidType m_toroidType;
};

RAYX_FN_ACC
inline Surface serializeToroid(ToroidSurface surface) {
    Surface ser;
    ser.m_type                            = SurfaceType::Toroid;
    ser.m_private_serialization_params[0] = surface.m_longRadius;
    ser.m_private_serialization_params[1] = surface.m_shortRadius;
    ser.m_private_serialization_params[2] = surface.m_toroidType;
    return ser;
}

RAYX_FN_ACC
inline ToroidSurface deserializeToroid(Surface ser) {
    ToroidSurface surface;
    surface.m_longRadius  = ser.m_private_serialization_params[0];
    surface.m_shortRadius = ser.m_private_serialization_params[1];
    surface.m_toroidType  = ser.m_private_serialization_params[2];
    return surface;
}

/////////////
// Plane
/////////////

struct PlaneSurface {
    // no parameters
};

///////////////////
// Cubic
///////////////////

struct CubicSurface {
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

    double m_b12;
    double m_b13;
    double m_b21;
    double m_b23;
    double m_b31;
    double m_b32;

    double m_psi;
};

struct Surface {
    std::variant<PlaneSurface, QuadricSurface, ToroidSurface, CubicSurface> m_surface = PlaneSurface{};
};

struct DesignElement;
enum class CylinderDirection { LongRadiusR, ShortRadiusRho };
Surface makeSurface(const DesignElement& dele);
Surface makeToroid(const DesignElement& dele);      //< creates a toroid from the parameters given in ` dele`.
Surface makeQuadric(const DesignElement& dele);     //< creates a quadric from the parameters given in ` dele`.
Surface makeCubic(const DesignElement& dele);       //< creates a cubic from the parameters given in ` dele`.
Surface makeEllipsoid(const DesignElement& dele);   //< creates a Ellipsoid from the parameters given in ` dele`.
Surface makeCone(const DesignElement& dele);        //< creates a Cone from the parameters given in ` dele`.
Surface makeCylinder(const DesignElement& dele);    //< creates a Cylinder from the parameters given in ` dele`.
Surface makeSphere(double radius);                  //< creates a sphere from the radius .
Surface makePlane();                                //< creates a plane surface
Surface makeParaboloid(const DesignElement& dele);  //< creates a Paraboloid from the parameters given in ` dele`.

// This prevents m_private_serialization_params from being used outside of this file - making them practically private.
#define m_private_serialization_params "m_private_serialization_params are private! Use the corresponding serialize & deserialize functions instead."

}  // namespace RAYX
