#pragma once

#include "Angle.h"
#include "Design/Curvature.h"
#include "Model/Curvature.h"
#include "ToModelTrait.h"
#include "Enums.h"

namespace rayx::detail {

inline model::QuadricCurvature toModelQuadric(const CylindricalCurvature& curvature) {
    auto cyl_direction     = toModel(curvature.direction());
    auto radius            = toModel(curvature.radius());
    auto incidence         = toModel(curvature.grazingIncAngle());
    auto entranceArmLength = toModel(curvature.entranceArmLength());
    auto exitArmLength     = toModel(curvature.exitArmLength());

    double a11 = 0, a33 = 0, a24 = 0;
    if (cyl_direction == CylinderDirection::LongRadiusR) {  // X-DIR
        a11 = 0;
        a33 = 1;
        a24 = -radius;
    } else {  // Z_DIR/ SHORT_RADIUS_RHO
        a11 = 1;
        a33 = 0;
        a24 = -radius;
    }
    int icurv = 1;
    if (a24 > 0) icurv = -1;  // Translated from RAY.FOR
    if (radius == 0) {
        if (cyl_direction == CylinderDirection::LongRadiusR) {
            radius = 2.0 / std::sin(incidence) / (1.0 / entranceArmLength + 1.0 / exitArmLength);
        } else {
            if (entranceArmLength == 0.0 || exitArmLength == 0.0 || incidence == 0.0) {
                radius = 0.0;
            } else {
                radius = 2.0 * std::sin(incidence) / (1.0 / entranceArmLength + 1.0 / exitArmLength);
            }
        }
    }

    return model::QuadricCurvature{
    return model::QuadricCurvature{
        .icurv = icurv,
        .a11   = a11,
        .a12   = 0,
        .a13   = 0,
        .a14   = 0,
        .a22   = 1,
        .a23   = 0,
        .a24   = a24,
        .a33   = a33,
        .a34   = 0,
        .a44   = 0,
    };
}

inline model::QuadricCurvature toModelQuadric(const SphericalCurvature& curvature) {
    return model::QuadricCurvature{
        .icurv = 1,
        .a11   = 1,
        .a12   = 0,
        .a13   = 0,
        .a14   = 0,
        .a22   = 1,
        .a23   = 0,
        .a24   = -toModel(curvature.radius()),
        .a33   = 1,
        .a34   = 0,
        .a44   = 0,
    };
}

inline model::QuadricCurvature toModelQuadric(const ParabolicCurvature& curvature) {
    auto ArmLength      = toModel(curvature.armLength());
    auto parameterP     = toModel(curvature.parameterP());
    auto parameterPType = toModel(curvature.parameterPType());

    auto grazingIncAngle = toModel(curvature.grazingIncAngle());
    auto a11             = toModel(curvature.parameterA11());

    double a24, a34, a44, y0, z0;
    //---------- Calculation will be outsourced ----------------
    int sign = parameterPType == ParabolicCurvatureType::Collimate ? 1 : -1;  // 0:collimate, 1:focussing

    double sin1 = std::sin(2 * grazingIncAngle);
    double cos1 = std::cos(2 * grazingIncAngle);  // Schaefers RAY-Book may have a different calculation

    y0 = ArmLength * sin1;
    z0 = ArmLength * cos1 * sign;

    a24 = -y0;
    a34 = -parameterP;
    a44 = std::pow(y0, 2) - 2 * parameterP * z0 - std::pow(parameterP, 2);
    //---------------------------- Serialization -------------------------------
    return model::QuadricCurvature{
    return model::QuadricCurvature{
        .icurv = 1,
        .a11   = a11,
        .a12   = 0,
        .a13   = 0,
        .a14   = 0,
        .a22   = 1.0,
        .a23   = 0,
        .a24   = a24,
        .a33   = 0,
        .a34   = a34,
        .a44   = a44,
    };
}

inline model::QuadricCurvature toModelQuadric(const ConicalCurvature& curvature) {
    double incidence         = toModel(curvature.grazingIncAngle());
    double entranceArmLength = toModel(curvature.entranceArmLength());
    double exitArmLength     = toModel(curvature.exitArmLength());

    double zl = toModel(curvature.totalLength());

    double ra = entranceArmLength;
    double rb = exitArmLength;

    double zl2    = std::pow(zl / 2, 2);
    double sth    = std::sin(incidence);
    double cth    = std::cos(incidence);
    double rmax1  = std::sqrt(zl2 + std::pow(ra, 2) - zl * ra * cth);
    double rmax2  = std::sqrt(zl2 + std::pow(rb, 2) + zl * rb * cth);
    double rmin1  = std::sqrt(zl2 + std::pow(ra, 2) + zl * ra * cth);
    double rmin2  = std::sqrt(zl2 + std::pow(rb, 2) - zl * rb * cth);
    double thmax  = asin(ra * sth / rmax1);
    double thmin  = asin(ra * sth / rmin1);
    double sthmax = sin(thmax);
    double sthmin = sin(thmin);

    double upstreamRadius_R     = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    double downstreamRadius_rho = 2 * sthmin / (1 / rmin1 + 1 / rmin2);

    auto cm = std::pow((upstreamRadius_R - downstreamRadius_rho) / zl, 2);

    int icurv  = 0;
    double a11 = 1 - cm;
    double a22 = 1 - 2 * cm;
    double a23 = std::sqrt(cm - cm * cm);
    double a24 = 0;  //  TODO correct default?

    if (a22 > 0) icurv = 1;
    if (a23 != 0) {
        a24 = -a23 * (upstreamRadius_R / std::sqrt(cm) - zl / 2);
    } else if (a23 == 0) {
        a24 = -upstreamRadius_R;
    }

    return model::QuadricCurvature{
        .icurv = icurv,
        .a11   = a11,
        .a12   = 0,
        .a13   = 0,
        .a14   = 0,
        .a22   = a22,
        .a23   = a23,
        .a24   = a24,
        .a33   = 0,
        .a34   = 0,
        .a44   = 0,
    };
}

inline model::QuadricCurvature toModelQuadric(const EllipticalCurvature& curvature) {
    auto entranceArmLength = toModel(curvature.entranceArmLength());
    auto exitArmLength     = toModel(curvature.exitArmLength());

    auto shortHalfAxisB     = toModel(curvature.shortHalfAxisB());
    auto longHalfAxisA      = toModel(curvature.longHalfAxisA());
    auto designGrazingAngle = toModel(curvature.designGrazingIncAngle());

    // if design angle not given, take incidenceAngle
    // calc y0
    double y0 = 0.0;
    if (longHalfAxisA > shortHalfAxisB) {
        if (designGrazingAngle > 0) {
            y0 =
                -std::pow(shortHalfAxisB, 2) * 1 / std::tan(designGrazingAngle) / std::sqrt(std::pow(longHalfAxisA, 2) - std::pow(shortHalfAxisB, 2));
        } else {
            y0 = -shortHalfAxisB;
        }
    } else {
        y0 = 0.0;
    }

    // calc z0
    double z0 = 0.0;
    if (entranceArmLength > exitArmLength && -shortHalfAxisB < y0) {
        z0 = longHalfAxisA * std::sqrt(std::pow(shortHalfAxisB, 2) - std::pow(y0, 2)) / shortHalfAxisB;
    } else if (entranceArmLength < exitArmLength && -shortHalfAxisB < y0) {
        z0 = -longHalfAxisA * std::sqrt(std::pow(shortHalfAxisB, 2) - std::pow(y0, 2)) / shortHalfAxisB;
    } else {
        z0 = 0.0;
    }

    // calc mt
    double mt = 0;  // tangent slope
    if (longHalfAxisA > 0.0 && y0 < 0.0) { mt = std::pow(shortHalfAxisB / longHalfAxisA, 2) * z0 / y0; }

    auto figureRotation = toModel(curvature.figureRotation());

    // calculate a11
    auto a11 = toModel(curvature.parameterA11());
    if (figureRotation == FigureRotation::Yes) {
        a11 = 1;
    } else if (figureRotation == FigureRotation::Plane) {
        a11 = 0;
    }

    // a33, 34, 44
    // a11 from rml file

    auto tangentAngle = std::atan(mt);
    auto a22          = std::pow(std::cos(tangentAngle), 2) + std::pow(shortHalfAxisB * std::sin(tangentAngle) / longHalfAxisA, 2);
    auto a23 =
        (std::pow(shortHalfAxisB, 2) - std::pow(longHalfAxisA, 2)) * std::cos(tangentAngle) * std::sin(tangentAngle) / std::pow(longHalfAxisA, 2);

    auto a24 = std::pow(shortHalfAxisB / longHalfAxisA, 2) * z0 * std::sin(tangentAngle) + y0 * std::cos(tangentAngle);
    auto a33 = std::pow(std::sin(tangentAngle), 2) + std::pow(shortHalfAxisB * std::cos(tangentAngle) / longHalfAxisA, 2);
    auto a34 = std::pow(shortHalfAxisB / longHalfAxisA, 2) * z0 * std::cos(tangentAngle) - y0 * std::sin(tangentAngle);
    auto a44 = -std::pow(shortHalfAxisB, 2) + std::pow(y0, 2) + std::pow(z0 * shortHalfAxisB / longHalfAxisA, 2);

    return model::QuadricCurvature{
        .icurv = 1,
        .a11   = a11,
        .a12   = 0,
        .a13   = 0,
        .a14   = 0,
        .a22   = a22,
        .a23   = a23,
        .a24   = a24,
        .a33   = a33,
        .a34   = a34,
        .a44   = a44,
    };
}

inline model::QuadricCurvature toModelQuadric(const QuadricCurvature& curvature) {
    return model::QuadricCurvature{
        .icurv = toModel(curvature.icurv()),
        .a11   = toModel(curvature.a11()),
        .a12   = toModel(curvature.a12()),
        .a13   = toModel(curvature.a13()),
        .a14   = toModel(curvature.a14()),
        .a22   = toModel(curvature.a22()),
        .a23   = toModel(curvature.a23()),
        .a24   = toModel(curvature.a24()),
        .a33   = toModel(curvature.a33()),
        .a34   = toModel(curvature.a34()),
        .a44   = toModel(curvature.a44()),
    };
}

inline model::ToroidialCurvature toModelToroidial(const ToroidialCurvature& curvature) {
    return model::ToroidialCurvature{
        .longRadius  = toModel(curvature.longRadius()),
        .shortRadius = toModel(curvature.shortRadius()),
        .toroidType  = toModel(curvature.toroidType()),
    };
}

inline model::CubicCurvature toModelCubic(const CubicCurvature& curvature) {
    return model::CubicCurvature{
        .a11 = toModel(curvature.a11()),
        .a12 = toModel(curvature.a12()),
        .a13 = toModel(curvature.a13()),
        .a14 = toModel(curvature.a14()),
        .a22 = toModel(curvature.a22()),
        .a23 = toModel(curvature.a23()),
        .a24 = toModel(curvature.a24()),
        .a33 = toModel(curvature.a33()),
        .a34 = toModel(curvature.a34()),
        .a44 = toModel(curvature.a44()),

        .b12 = toModel(curvature.b12()),
        .b13 = toModel(curvature.b13()),
        .b21 = toModel(curvature.b21()),
        .b23 = toModel(curvature.b23()),
        .b31 = toModel(curvature.b31()),
        .b32 = toModel(curvature.b32()),

        .psi = toModel(curvature.psi()),
    };
}

template <>
struct ToModel<QuadricCurvature> {
    static model::Curvature apply(const QuadricCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<ToroidialCurvature> {
    static model::Curvature apply(const ToroidialCurvature& curvature) { return toModelToroidial(curvature); }
};

template <>
struct ToModel<CubicCurvature> {
    static model::Curvature apply(const CubicCurvature& curvature) { return toModelCubic(curvature); }
};

template <>
struct ToModel<CylindricalCurvature> {
    static model::Curvature apply(const CylindricalCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<SphericalCurvature> {
    static model::Curvature apply(const SphericalCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<ParabolicCurvature> {
    static model::Curvature apply(const ParabolicCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<ConicalCurvature> {
    static model::Curvature apply(const ConicalCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<EllipticalCurvature> {
    static model::Curvature apply(const EllipticalCurvature& curvature) { return toModelQuadric(curvature); }
};

template <>
struct ToModel<Curvature> {
    static model::Curvature apply(const Curvature& curvature) {
        return std::visit([](const auto& curv) { return toModel(curv); }, curvature);
    }
};

}  // namespace rayx::detail
