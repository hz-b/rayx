#pragma once

#include "Debug/Debug.h"

namespace rayx {
enum class GratingMount { Deviation, Incidence };
enum class ParaboloidType { Focussing, Collimate };
enum class CurvatureType {
    Plane,
    Toroidal,
    Spherical,
    Cubic,
    Cone,
    Cylinder,
    Ellipsoid,
    Paraboloid,
    Quadric,
    RzpSphere
};  // order is crucial for xml prser
enum class BehaviourType { Mirror, Grating, Slit, Rzp, ImagePlane, Crystal, Foil };
enum class FigureRotation { Yes, Plane, A11 };

// the direction of a plane, either XY or XZ. This is only used in the parsing.
// Inside of the shader, every plane-shaped object lies in its XZ plane.
// Per default every element has DesignPlane::XZ, but ImagePlane and Slit have DesignPlane::XY.
// Thus they need a bit of extra handling to convert them to the shaders XZ plane.
enum class DesignPlane { XY, XZ };

}  // namespace rayx
