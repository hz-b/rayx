#pragma once

#include <cuda/std/variant>

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

//////////////
// Toroid
/////////////

#define ToroidType double
constexpr ToroidType TOROID_TYPE_CONVEX = 0;
constexpr ToroidType TOROID_TYPE_CONCAVE = 1;

struct ToroidSurface {
    double m_longRadius;
    double m_shortRadius;
    ToroidType m_toroidType;
};

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
    cuda::std::variant<PlaneSurface, QuadricSurface, ToroidSurface, CubicSurface> m_surface = PlaneSurface{};
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
