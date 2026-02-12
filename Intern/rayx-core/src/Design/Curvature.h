#pragma once

#include <variant>

#include "Angle.h"

namespace rayx {

// TODO: find sensible defaults for all curvature types
// TODO: sensible validation for all curvature types
// TODO: find alternative representation for curvature types
// TODO: require mandatory arguments in constructors

struct QuadricCurvature {
    RAYX_PROPERTY(QuadricCurvature, int, icurv);
    RAYX_PROPERTY(QuadricCurvature, double, a11);
    RAYX_PROPERTY(QuadricCurvature, double, a12);
    RAYX_PROPERTY(QuadricCurvature, double, a13);
    RAYX_PROPERTY(QuadricCurvature, double, a14);
    RAYX_PROPERTY(QuadricCurvature, double, a22);
    RAYX_PROPERTY(QuadricCurvature, double, a23);
    RAYX_PROPERTY(QuadricCurvature, double, a24);
    RAYX_PROPERTY(QuadricCurvature, double, a33);
    RAYX_PROPERTY(QuadricCurvature, double, a34);
    RAYX_PROPERTY(QuadricCurvature, double, a44);
};

enum class ToroidType {
    Convex,
    Concave,
};

struct ToroidialCurvature {
    RAYX_PROPERTY(ToroidialCurvature, double, longRadius);
    RAYX_PROPERTY(ToroidialCurvature, double, shortRadius);
    RAYX_PROPERTY(ToroidialCurvature, ToroidType, toroidType);
};

struct CubicCurvature {
    // RAYX_PROPERTY(CubicCurvature, int, icurv);
    RAYX_PROPERTY(CubicCurvature, double, a11);
    RAYX_PROPERTY(CubicCurvature, double, a12);
    RAYX_PROPERTY(CubicCurvature, double, a13);
    RAYX_PROPERTY(CubicCurvature, double, a14);
    RAYX_PROPERTY(CubicCurvature, double, a22);
    RAYX_PROPERTY(CubicCurvature, double, a23);
    RAYX_PROPERTY(CubicCurvature, double, a24);
    RAYX_PROPERTY(CubicCurvature, double, a33);
    RAYX_PROPERTY(CubicCurvature, double, a34);
    RAYX_PROPERTY(CubicCurvature, double, a44);

    RAYX_PROPERTY(CubicCurvature, double, b12);
    RAYX_PROPERTY(CubicCurvature, double, b13);
    RAYX_PROPERTY(CubicCurvature, double, b21);
    RAYX_PROPERTY(CubicCurvature, double, b23);
    RAYX_PROPERTY(CubicCurvature, double, b31);
    RAYX_PROPERTY(CubicCurvature, double, b32);

    RAYX_PROPERTY(CubicCurvature, double, psi);
};

enum class FigureRotation { No, Yes, Plane };

struct EllipticalCurvature {
    RAYX_PROPERTY(EllipticalCurvature, double, shortHalfAxisB)              = 0.0;
    RAYX_PROPERTY(EllipticalCurvature, double, longHalfAxisA)               = 0.0;
    RAYX_PROPERTY(EllipticalCurvature, double, entranceArmLength)           = 0.0;
    RAYX_PROPERTY(EllipticalCurvature, double, exitArmLength)               = 0.0;
    RAYX_NESTED_PROPERTY(EllipticalCurvature, Angle, designGrazingIncAngle) = Rad(0.0);
    RAYX_PROPERTY(EllipticalCurvature, FigureRotation, figureRotation)      = FigureRotation::No;
    RAYX_PROPERTY(EllipticalCurvature, double, parameterA11)                = 1.0;
};

struct ConicalCurvature {
    RAYX_NESTED_PROPERTY(ConicalCurvature, Angle, grazingIncAngle) = Rad(0.0);
    RAYX_PROPERTY(ConicalCurvature, double, entranceArmLength)     = 0.0;
    RAYX_PROPERTY(ConicalCurvature, double, exitArmLength)         = 0.0;
    RAYX_PROPERTY(ConicalCurvature, double, totalLength)           = 0.0;
};

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

struct CylindricalCurvature {
    RAYX_PROPERTY(CylindricalCurvature, CylinderDirection, direction)  = CylinderDirection::LongRadiusR;
    RAYX_PROPERTY(CylindricalCurvature, double, radius)                = 0.0;
    RAYX_NESTED_PROPERTY(CylindricalCurvature, Angle, grazingIncAngle) = Rad(0.0);
    RAYX_PROPERTY(CylindricalCurvature, double, entranceArmLength)     = 0.0;
    RAYX_PROPERTY(CylindricalCurvature, double, exitArmLength)         = 0.0;
};

struct SphericalCurvature {
    RAYX_PROPERTY(SphericalCurvature, double, radius) = 0.0;
};

enum class ParabolicCurvatureType { Collimate, Focussing };

struct ParabolicCurvature {
    RAYX_PROPERTY(ParabolicCurvature, double, armLength)                      = 0.0;
    RAYX_PROPERTY(ParabolicCurvature, double, parameterP)                     = 0.0;
    RAYX_PROPERTY(ParabolicCurvature, ParabolicCurvatureType, parameterPType) = ParabolicCurvatureType::Collimate;
    RAYX_NESTED_PROPERTY(ParabolicCurvature, Angle, grazingIncAngle)          = Rad(0.0);
    RAYX_PROPERTY(ParabolicCurvature, double, parameterA11)                   = 1.0;
};

using Curvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature, EllipticalCurvature, ConicalCurvature, CylindricalCurvature,
                               SphericalCurvature, ParabolicCurvature>;

}  // namespace rayx
