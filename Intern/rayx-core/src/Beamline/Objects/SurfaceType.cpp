#include "SurfaceType.h"

#include "Beamline/Beamline.h"
#include "DesignElement/DesignElement.h"

namespace RAYX {

Surface makeSurface(const DesignElement& dele) {
    auto surface = dele.getCurvatureType();
    if (surface == CurvatureType::Plane) {
        return makePlane();
    } else if (surface == CurvatureType::Toroidal) {
        return makeToroid(dele);
    } else if (surface == CurvatureType::Spherical) {
        return makeSphere(dele.getRadius());
    } else if (surface == CurvatureType::RzpSphere) {
        return makeSphere(dele.getLongRadius());
    } else if (surface == CurvatureType::Cone) {
        return makeCone(dele);
    } else if (surface == CurvatureType::Cylinder) {
        return makeCylinder(dele);
    } else if (surface == CurvatureType::Ellipsoid) {
        return makeEllipsoid(dele);
    } else if (surface == CurvatureType::Paraboloid) {
        return makeParaboloid(dele);
    } else {
        return serializePlaneXZ();
    }
}

Surface makePlane() { return serializePlaneXZ(); }

Surface makeCylinder(const DesignElement& dele) {
    auto cyl_direction = dele.getRadiusDirection();
    auto radius = dele.getRadius();
    auto incidence = dele.getGrazingIncAngle();
    auto entranceArmLength = dele.getEntranceArmLength();
    auto exitArmLength = dele.getExitArmLength();

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
            radius = 2.0 / incidence.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);
        } else {
            if (entranceArmLength == 0.0 || exitArmLength == 0.0 || incidence.rad == 0.0) {
                radius = 0.0;
            } else {
                radius = 2.0 * incidence.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);
            }
        }
    }

    return serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = a24,
        .m_a33 = a33,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Surface makeCone(const DesignElement& dele) {
    auto incidence = dele.getGrazingIncAngle();
    double entranceArmLength = dele.getEntranceArmLength();
    double exitArmLength = dele.getExitArmLength();

    double zl = dele.getTotalLength();

    double ra = entranceArmLength;
    double rb = exitArmLength;

    double zl2 = pow(zl / 2, 2);
    double sth = incidence.sin();
    double cth = incidence.cos();
    double rmax1 = sqrt(zl2 + pow(ra, 2) - zl * ra * cth);
    double rmax2 = sqrt(zl2 + pow(rb, 2) + zl * rb * cth);
    double rmin1 = sqrt(zl2 + pow(ra, 2) + zl * ra * cth);
    double rmin2 = sqrt(zl2 + pow(rb, 2) - zl * rb * cth);
    double thmax = asin(ra * sth / rmax1);
    double thmin = asin(ra * sth / rmin1);
    double sthmax = sin(thmax);
    double sthmin = sin(thmin);

    double upstreamRadius_R = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    double downstreamRadius_rho = 2 * sthmin / (1 / rmin1 + 1 / rmin2);

    auto cm = pow((upstreamRadius_R - downstreamRadius_rho) / zl, 2);

    int icurv = 0;
    double a11 = 1 - cm;
    double a22 = 1 - 2 * cm;
    double a23 = sqrt(cm - cm * cm);
    double a24 = 0;  //  TODO correct default?

    if (a22 > 0) icurv = 1;
    if (a23 != 0) {
        a24 = -a23 * (upstreamRadius_R / sqrt(cm) - zl / 2);
    } else if (a23 == 0) {
        a24 = -upstreamRadius_R;
    }

    return serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = a22,
        .m_a23 = a23,
        .m_a24 = a24,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Surface makeEllipsoid(const DesignElement& dele) {
    auto entranceArmLength = dele.getEntranceArmLength();
    auto exitArmLength = dele.getExitArmLength();

    auto shortHalfAxisB = dele.getShortHalfAxisB();
    auto longHalfAxisA = dele.getLongHalfAxisA();
    auto designGrazingAngle = dele.getDesignGrazingIncAngle();

    // if design angle not given, take incidenceAngle
    // calc y0
    double y0 = 0.0;
    if (longHalfAxisA > shortHalfAxisB) {
        if (designGrazingAngle.rad > 0) {
            y0 = -pow(shortHalfAxisB, 2) * 1 / designGrazingAngle.tan() / sqrt(pow(longHalfAxisA, 2) - pow(shortHalfAxisB, 2));
        } else {
            y0 = -shortHalfAxisB;
        }
    } else {
        y0 = 0.0;
    }

    // calc z0
    double z0 = 0.0;
    if (entranceArmLength > exitArmLength && -shortHalfAxisB < y0) {
        z0 = longHalfAxisA * sqrt(pow(shortHalfAxisB, 2) - pow(y0, 2)) / shortHalfAxisB;
    } else if (entranceArmLength < exitArmLength && -shortHalfAxisB < y0) {
        z0 = -longHalfAxisA * sqrt(pow(shortHalfAxisB, 2) - pow(y0, 2)) / shortHalfAxisB;
    } else {
        z0 = 0.0;
    }

    // calc mt
    double mt = 0;  // tangent slope
    if (longHalfAxisA > 0.0 && y0 < 0.0) {
        mt = pow(shortHalfAxisB / longHalfAxisA, 2) * z0 / y0;
    }

    auto figureRotation = dele.getFigureRotation();

    // calculate a11
    auto a11 = dele.getParameterA11();
    if (figureRotation == FigureRotation::Yes) {
        a11 = 1;
    } else if (figureRotation == FigureRotation::Plane) {
        a11 = 0;
    }

    // a33, 34, 44
    // a11 from rml file

    auto tangentAngle = Rad(atan(mt));
    auto a22 = pow(tangentAngle.cos(), 2) + pow(shortHalfAxisB * tangentAngle.sin() / longHalfAxisA, 2);
    auto a23 = (pow(shortHalfAxisB, 2) - pow(longHalfAxisA, 2)) * tangentAngle.cos() * tangentAngle.sin() / pow(longHalfAxisA, 2);

    auto a24 = pow(shortHalfAxisB / longHalfAxisA, 2) * z0 * tangentAngle.sin() + y0 * tangentAngle.cos();
    auto a33 = pow(tangentAngle.sin(), 2) + pow(shortHalfAxisB * tangentAngle.cos() / longHalfAxisA, 2);
    auto a34 = pow(shortHalfAxisB / longHalfAxisA, 2) * z0 * tangentAngle.cos() - y0 * tangentAngle.sin();
    auto a44 = -pow(shortHalfAxisB, 2) + pow(y0, 2) + pow(z0 * shortHalfAxisB / longHalfAxisA, 2);

    return serializeQuadric({
        .m_icurv = 1,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = a22,
        .m_a23 = a23,
        .m_a24 = a24,
        .m_a33 = a33,
        .m_a34 = a34,
        .m_a44 = a44,
    });
}

Surface makeSphere(double radius) {
    return serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 1,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = -radius,
        .m_a33 = 1,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Surface makeToroid(const DesignElement& dele) {
    return serializeToroid({
        .m_longRadius = dele.getLongRadius(),
        .m_shortRadius = dele.getShortRadius(),
        .m_toroidType = TOROID_TYPE_CONCAVE,
    });
}

Surface makeParaboloid(const DesignElement& dele) {
    auto ArmLength = dele.getArmLength();
    auto parameterP = dele.getParameterP();
    auto parameterPType = dele.getParameterPType();

    auto grazingIncAngle = dele.getGrazingIncAngle();
    auto a11 = dele.getParameterA11();

    double a24, a34, a44, y0, z0;
    //---------- Calculation will be outsourced ----------------
    int sign = parameterPType == 0 ? 1 : -1;  // 0:collimate, 1:focussing

    double sin1 = sin(2 * grazingIncAngle.rad);
    double cos1 = cos(2 * grazingIncAngle.rad);  // Schaefers RAY-Book may have a different calculation

    y0 = ArmLength * sin1;
    z0 = ArmLength * cos1 * sign;

    a24 = -y0;
    a34 = -parameterP;
    a44 = pow(y0, 2) - 2 * parameterP * z0 - pow(parameterP, 2);
    //-----------------------------------------------------------
    return serializeQuadric({
        .m_icurv = 1,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1.0,
        .m_a23 = 0,
        .m_a24 = a24,
        .m_a33 = 0,
        .m_a34 = a34,
        .m_a44 = a44,
    });
}

Surface makeQuadric(const DesignElement& dele) { return dele.getExpertsOptics(); }

Surface makeCubic(const DesignElement& dele) { return dele.getExpertsCubic(); }

}  // namespace RAYX