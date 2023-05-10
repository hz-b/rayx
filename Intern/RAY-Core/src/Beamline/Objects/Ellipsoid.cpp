#include "Ellipsoid.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeEllipsoid(const DesignObject& dobj) {
    auto entranceArmLength = dobj.parseEntranceArmLength();
    auto exitArmLength = dobj.parseExitArmLength();
    auto shortHalfAxisB = dobj.parseShortHalfAxisB();
    auto longHalfAxisA = dobj.parseLongHalfAxisA();
    auto designGrazingAngle = dobj.parseDesignGrazingIncAngle();

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

    auto figureRotation = dobj.parseFigureRotation();

    // calculate a11
    auto a11 = dobj.parseParameterA11();
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

    auto surface = serializeQuadric({
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
    auto behaviour = serializeMirror();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
