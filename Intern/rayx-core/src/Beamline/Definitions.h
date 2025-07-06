#pragma once

#include "Debug/Debug.h"

namespace RAYX {
enum class GratingMount { Deviation, Incidence };
enum class ParaboloidType { Focussing, Collimate };
enum class CurvatureType {
    Plane,
    Toroidal,
    Spherical,
    SphericalVolume,
    Cubic,
    Cone,
    Cylinder,
    Ellipsoid,
    Paraboloid,
    Quadric,
    RzpSphere
};  // order is crucial for xml prser
enum class BehaviourType { Mirror, Grating, Slit, Rzp, ImagePlane, Crystal, Foil, Lens };
enum class FigureRotation { Yes, Plane, A11 };

// the direction of a plane, either XY or XZ. This is only used in the parsing.
// Inside of the shader, every plane-shaped object lies in its XZ plane.
// Per default every element has DesignPlane::XZ, but ImagePlane and Slit have DesignPlane::XY.
// Thus they need a bit of extra handling to convert them to the shaders XZ plane.
enum class DesignPlane { XY, XZ };

inline std::string curvatureTypeToString(const CurvatureType type) {
    switch (type) {
        case CurvatureType::Plane:
            return "Plane";
        case CurvatureType::Toroidal:
            return "Toroidal";
        case CurvatureType::Spherical:
            return "Spherical";
        case CurvatureType::Cubic:
            return "Cubic";
        case CurvatureType::Cone:
            return "Cone";
        case CurvatureType::Cylinder:
            return "Cylinder";
        case CurvatureType::Ellipsoid:
            return "Ellipsoid";
        case CurvatureType::Paraboloid:
            return "Paraboloid";
        case CurvatureType::Quadric:
            return "Quadric";
        case CurvatureType::RzpSphere:
            return "RzpSphere";
        default:
            RAYX_EXIT << "error: unknown curvature type: " << static_cast<int>(type);
            return "";
    }
}

inline std::string behaviourTypeToString(const BehaviourType type) {
    switch (type) {
        case BehaviourType::Mirror:
            return "Mirror";
        case BehaviourType::Grating:
            return "Grating";
        case BehaviourType::Slit:
            return "Slit";
        case BehaviourType::Rzp:
            return "Rzp";
        case BehaviourType::ImagePlane:
            return "ImagePlane";
        default:
            RAYX_EXIT << "error: unknown behaviour type: " << static_cast<int>(type);
            return "";
    }
}

}  // namespace RAYX
