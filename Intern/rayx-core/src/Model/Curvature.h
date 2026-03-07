#pragma once

#include <variant>

#include "Common/Enums.h"

namespace rayx::detail::model {

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

struct ToroidialCurvature {
    double longRadius;
    double shortRadius;
    ToroidType toroidType;
};

struct CubicCurvature {
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

using Curvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature>;

}  // namespace rayx::detail::model
