#include "Ellipsoid.h"

#include "Debug.h"

namespace RAYX {

Ellipsoid::Ellipsoid(const DesignObject& dobj) : OpticalElement(dobj) {
    m_incidence = dobj.parseGrazingIncAngle();
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();
    m_a11 = dobj.parseParameterA11();
    m_shortHalfAxisB = dobj.parseShortHalfAxisB();
    m_longHalfAxisA = dobj.parseLongHalfAxisA();
    m_designGrazingAngle = dobj.parseDesignGrazingIncAngle();
    // set geometry

    m_offsetY0 = 0;  // what is this for? RAY.FOR: "only !=0 in case of Monocapillary"

    m_figureRotation = dobj.parseFigureRotation();

    // if design angle not given, take incidenceAngle
    calculateCenterFromHalfAxes(m_designGrazingAngle);

    // calculate half axis C
    if (m_figureRotation == FigureRotation::Yes) {
        m_halfAxisC = m_shortHalfAxisB;  // sqrt(pow(m_shortHalfAxisB, 2) / 1);
                                         // devided by 1??
        m_a11 = 1;
    } else if (m_figureRotation == FigureRotation::Plane) {
        m_halfAxisC = INFINITY;
        m_a11 = 0;
    } else {
        m_halfAxisC = sqrt(pow(m_shortHalfAxisB, 2) / m_a11);
    }

    RAYX_VERB << "A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB << ", C= " << m_halfAxisC;

    // a33, 34, 44
    // a11 from rml file

    m_a22 = pow(cos(m_tangentAngle), 2) + pow(m_shortHalfAxisB * sin(m_tangentAngle) / m_longHalfAxisA, 2);
    m_a23 = (pow(m_shortHalfAxisB, 2) - pow(m_longHalfAxisA, 2)) * cos(m_tangentAngle) * sin(m_tangentAngle) / pow(m_longHalfAxisA, 2);

    m_a24 = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 * sin(m_tangentAngle) + m_y0 * cos(m_tangentAngle);
    m_a33 = pow(sin(m_tangentAngle), 2) + pow(m_shortHalfAxisB * cos(m_tangentAngle) / m_longHalfAxisA, 2);
    m_a34 = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 * cos(m_tangentAngle) - m_y0 * sin(m_tangentAngle);
    m_a44 = -pow(m_shortHalfAxisB, 2) + pow(m_y0, 2) + pow(m_z0 * m_shortHalfAxisB / m_longHalfAxisA, 2);

    RAYX_VERB << "alpha1: " << m_tangentAngle << "; in Degree: " << radToDeg(m_tangentAngle);
    RAYX_VERB << "m_y0: " << m_y0;
    RAYX_VERB << "m_z0: " << m_z0;
    RAYX_VERB << "m_a11: " << m_a11;
    RAYX_VERB << "m_a22: " << m_a22;
    RAYX_VERB << "m_a23: " << m_a23;
    RAYX_VERB << "m_a24 (m_radius): " << m_a24;
    RAYX_VERB << "m_a33: " << m_a33;
    RAYX_VERB << "m_a34: " << m_a34;
    RAYX_VERB << "m_a44: " << m_a44;

    double icurv = 1;
    Material mat = dobj.parseMaterial();
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{m_a11, 0, 0, 0,              //
                                                      icurv, m_a22, m_a23, m_a24,  //
                                                      0, 0, m_a33, m_a34,          //
                                                      7, 0, matd, m_a44}));
}

void Ellipsoid::calculateCenterFromHalfAxes(double angle) {
    // TODO: is mt = 0 a good default for the case that it'll never be set?
    double mt = 0;  // tangent slope
    if (m_longHalfAxisA > m_shortHalfAxisB) {
        if (angle > 0) {
            m_y0 = -pow(m_shortHalfAxisB, 2) * 1 / tan(angle) / sqrt(pow(m_longHalfAxisA, 2) - pow(m_shortHalfAxisB, 2));
        } else {
            m_y0 = -m_shortHalfAxisB;
        }
    } else {
        m_y0 = 0.0;
    }
    if (m_entranceArmLength > m_exitArmLength && -m_shortHalfAxisB < m_y0) {
        m_z0 = m_longHalfAxisA * sqrt(pow(m_shortHalfAxisB, 2) - pow(m_y0, 2)) / m_shortHalfAxisB;
    } else if (m_entranceArmLength < m_exitArmLength && -m_shortHalfAxisB < m_y0) {
        m_z0 = -m_longHalfAxisA * sqrt(pow(m_shortHalfAxisB, 2) - pow(m_y0, 2)) / m_shortHalfAxisB;
    } else {
        m_z0 = 0.0;
    }
    if (m_longHalfAxisA > 0.0 && m_y0 < 0.0) {
        mt = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 / m_y0;
    }
    m_tangentAngle = (atan(mt));
    RAYX_VERB << "Z0 = " << m_z0 << ", Y0= " << m_y0 << ", tangentAngle= " << m_tangentAngle;
}

/**
 *  caclulates the half axes, tangent angle and the center of the ellipsoid (z0,
 * y0) from the incidence angle, entrance and exit arm lengths, see ELLPARAM in
 * RAYX.FOR
 */
void Ellipsoid::calcHalfAxes() {
    double theta = m_incidence;  // designGrazingIncidenceAngle always equal to
                                 // alpha (grazingIncidenceAngle)??
    if (theta > PI / 2) {
        theta = PI / 2;
    }
    double a = 0.5 * (m_entranceArmLength + m_exitArmLength);

    double angle = atan(tan(theta) * (m_entranceArmLength - m_exitArmLength) / (m_entranceArmLength + m_exitArmLength));
    m_y0 = m_entranceArmLength * sin(theta - angle);
    double b = a * m_y0 * tan(angle);
    b = 0.25 * pow(m_y0, 4) + pow(b, 2);
    b = sqrt(0.5 * m_y0 * m_y0 + sqrt(b));

    m_z0 = 0;  // center of ellipsoid y0,z0
    if (b != 0) {
        m_z0 = (a / b) * (a / b) * m_y0 * tan(angle);
    }

    // << ellparam in RAY.for to calculate long and short half axis A and B
    m_longHalfAxisA = a;
    m_shortHalfAxisB = b;

    // calculate half axis C
    if (m_figureRotation == FigureRotation::Yes) {
        m_halfAxisC = sqrt(pow(m_shortHalfAxisB, 2) / 1);  // devided by 1??
        m_a11 = 1;
    } else if (m_figureRotation == FigureRotation::Plane) {
        m_halfAxisC = INFINITY;
        m_a11 = 0;
    } else {
        m_halfAxisC = sqrt(pow(m_shortHalfAxisB, 2) / m_a11);
    }
    m_tangentAngle = angle;
    RAYX_VERB << "A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB << ", C= " << m_halfAxisC << ", angle = " << radToDeg(m_tangentAngle);
}

double Ellipsoid::getRadius() const { return m_a24; }

double Ellipsoid::getExitArmLength() const { return m_exitArmLength; }

double Ellipsoid::getEntranceArmLength() const { return m_entranceArmLength; }

double Ellipsoid::getY0() const { return m_y0; }

double Ellipsoid::getZ0() const { return m_z0; }
double Ellipsoid::getIncidenceAngle() const { return m_incidence; }

double Ellipsoid::getShortHalfAxisB() const { return m_shortHalfAxisB; }
double Ellipsoid::getLongHalfAxisA() const { return m_longHalfAxisA; }
double Ellipsoid::getOffsetY0() const { return m_offsetY0; }
double Ellipsoid::getTangentAngle() const { return m_tangentAngle; }
double Ellipsoid::getA34() const { return m_a34; }
double Ellipsoid::getA33() const { return m_a33; }
double Ellipsoid::getA44() const { return m_a44; }
double Ellipsoid::getHalfAxisC() const { return m_halfAxisC; }

glm::dmat4x4 Ellipsoid::getElementParameters() const {
    return {sin(m_tangentAngle),
            cos(m_tangentAngle),
            m_y0,
            m_z0,  //
            double(m_figureRotation),
            0,
            0,
            0,  //
            0,
            0,
            0,
            0,  //
            0,
            0,
            0,
            0};
}

}  // namespace RAYX
