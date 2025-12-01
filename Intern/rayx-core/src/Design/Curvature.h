#pragma once

#include <variant>

namespace rayx {

struct QuadricCurvature {
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

struct ToroidialCurvature {
    double m_longRadius;
    double m_shortRadius;
    ToroidType m_toroidType;
};

struct CubicCurvature {
    // int m_icurv;
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

struct EllipticalCurvature {
    double radiusX = 0.0;
    double radiusZ = 0.0;
};

struct ConicalCurvature {
    double radius = 0.0;
};

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

struct CylindricalCurvature {
    double radius = 0.0;
    CylinderDirection direction = CylinderDirection::LongRadiusR;
};

struct SphericalCurvature {
    double radius = 0.0;
};

struct ParabolicCurvature {
    double focalLength = 0.0;
};

using Curvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature, EllipticalCurvature, ConicalCurvature, CylindricalCurvature,
                               SphericalCurvature, ParabolicCurvature>;



} // namespace rayx
