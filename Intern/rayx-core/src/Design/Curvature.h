#pragma once

#include <variant>

#include "Angle.h"

namespace rayx::design {

struct QuadricCurvature {
    int icurv;
    double a11;
    double a12;
    double a13;
    double a14;
    double a22;
    double a23;
    double a24;
    double a33;
    double a34;
    double a44;
};

enum class ToroidType {
    Convex,
    Concave,
};

struct ToroidialCurvature {
    double longRadius;
    double shortRadius;
    ToroidType toroidType;
};

struct CubicCurvature {
    // int icurv;
    double a11;
    double a12;
    double a13;
    double a14;
    double a22;
    double a23;
    double a24;
    double a33;
    double a34;
    double a44;

    double b12;
    double b13;
    double b21;
    double b23;
    double b31;
    double b32;

    double psi;
};

enum class FigureRotation { No, Yes, Plane };

struct EllipticalCurvature {
    double shortHalfAxisB         = 0.0;
    double longHalfAxisA          = 0.0;
    double entranceArmLength      = 0.0;
    double exitArmLength          = 0.0;
    Angle designGrazingIncAngle   = Radians(0.0);
    FigureRotation figureRotation = FigureRotation::No;
    double parameterA11           = 1.0;
};

struct ConicalCurvature {
    Angle grazingIncAngle    = Radians(0.0);
    double entranceArmLength = 0.0;
    double exitArmLength     = 0.0;
    double totalLength       = 0.0;
};

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

struct CylindricalCurvature {
    CylinderDirection direction = CylinderDirection::LongRadiusR;
    double radius               = 0.0;
    Angle grazingIncAngle       = Radians(0.0);
    double entranceArmLength    = 0.0;
    double exitArmLength        = 0.0;
};

struct SphericalCurvature {
    double radius = 0.0;
};

enum class ParabolicCurvatureType { Collimate, Focussing };

struct ParabolicCurvature {
    double armLength                      = 0.0;
    double parameterP                     = 0.0;
    ParabolicCurvatureType parameterPType = ParabolicCurvatureType::Collimate;
    Angle grazingIncAngle                 = Radians(0.0);
    double parameterA11                   = 1.0;
};

using Curvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature, EllipticalCurvature, ConicalCurvature, CylindricalCurvature,
                               SphericalCurvature, ParabolicCurvature>;

}  // namespace rayx::design
