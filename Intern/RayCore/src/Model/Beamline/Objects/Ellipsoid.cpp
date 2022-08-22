#include "Ellipsoid.h"

#include "Debug.h"

namespace RAYX {

/**
 * Angles given in degree and stored in rad. Initializes transformation matrices
 * from position and orientation, and parameters for the quadric in super class
 * (surface). stores mirror-specific parameters in this class.
 *
 * @param width             width of the mirror (x-dimension)
 * @param height            height of the mirror (z-dimension)
 * @param azimuthalAngle        azimuthal angle of object (rotation in xy plane
 * with respect to previous element) in rad
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param figRot            figure of rotation (0 = yes, 1 = plane, 2 = no,
 * short half axis C)??
 * @param a_11              a_11 in quadric equation
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat               material (See Material.h)
 *
 */
// User-defined Parm constructor
Ellipsoid::Ellipsoid(const char* name,
                     OpticalElement::GeometricalShape geometricalShape,
                     const double width, const double height,
                     const double azimuthalAngle, glm::dvec4 position,
                     const double LongHalfAxisA, const double ShortHalfAxisB,
                     const double DesignAngle, glm::dmat4x4 orientation,
                     const double grazingIncidence,
                     const double entranceArmLength, const double exitArmLength,
                     FigureRotation figRot, const double a_11,
                     const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength),
      m_a11(a_11),
      m_shortHalfAxisB(ShortHalfAxisB),
      m_longHalfAxisA(LongHalfAxisA),
      m_designGrazingAngle(degToRad(DesignAngle)) {
    RAYX_LOG << name;
    m_offsetY0 =
        0;  // what is this for? RAY.FOR: "only !=0 in case of Monocapillary"

    m_figureRotation = figRot;

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

    RAYX_LOG << "A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB
             << ", C= " << m_halfAxisC;

    // a33, 34, 44
    // a11 from rml file

    m_a22 = pow(cos(m_tangentAngle), 2) +
            pow(m_shortHalfAxisB * sin(m_tangentAngle) / m_longHalfAxisA, 2);
    m_a23 = (pow(m_shortHalfAxisB, 2) - pow(m_longHalfAxisA, 2)) *
            cos(m_tangentAngle) * sin(m_tangentAngle) / pow(m_longHalfAxisA, 2);

    m_a24 = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 *
                sin(m_tangentAngle) +
            m_y0 * cos(m_tangentAngle);
    m_a33 = pow(sin(m_tangentAngle), 2) +
            pow(m_shortHalfAxisB * cos(m_tangentAngle) / m_longHalfAxisA, 2);
    m_a34 = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 *
                cos(m_tangentAngle) -
            m_y0 * sin(m_tangentAngle);
    m_a44 = -pow(m_shortHalfAxisB, 2) + pow(m_y0, 2) +
            pow(m_z0 * m_shortHalfAxisB / m_longHalfAxisA, 2);

    RAYX_D_LOG << "alpha1: " << m_tangentAngle
             << "; in Degree: " << radToDeg(m_tangentAngle);
    RAYX_D_LOG << "m_y0: " << m_y0;
    RAYX_D_LOG << "m_z0: " << m_z0;
    RAYX_D_LOG << "m_a11: " << m_a11;
    RAYX_D_LOG << "m_a22: " << m_a22;
    RAYX_D_LOG << "m_a23: " << m_a23;
    RAYX_D_LOG << "m_a24 (m_radius): " << m_a24;
    RAYX_D_LOG << "m_a33: " << m_a33;
    RAYX_D_LOG << "m_a34: " << m_a34;
    RAYX_D_LOG << "m_a44: " << m_a44;

    double icurv = 1;
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(
        std::array<double, 4 * 4>{m_a11, 0, 0, 0,         //
                                  icurv, m_a22, m_a23, m_a24,  //
                                  0, 0, m_a33, m_a34,  //
                                  7, 0, matd, m_a44}));
    setElementParameters({sin(m_tangentAngle),cos(m_tangentAngle), m_y0,
                          m_z0,                               //
                          double(m_figureRotation), 0, 0, 0,  //
                          0, 0, 0, 0,                         //
                          0, 0, 0, 0});
}

/*
void Ellipsoid::calcRadius() {
    double theta = m_alpha; // grazing incidence in rad
    m_radius = 2.0/sin(theta) / (1.0 / m_entranceArmLength + 1.0 /
m_exitArmLength);
}*/

void Ellipsoid::calculateCenterFromHalfAxes(double angle) {
    // TODO: is mt = 0 a good default for the case that it'll never be set?
    double mt = 0;  // tangent slope
    if (m_longHalfAxisA > m_shortHalfAxisB) {
        if (angle > 0) {
            m_y0 = -pow(m_shortHalfAxisB, 2) * 1 / tan(angle) /
                   sqrt(pow(m_longHalfAxisA, 2) - pow(m_shortHalfAxisB, 2));
        } else {
            m_y0 = -m_shortHalfAxisB;
        }
    } else {
        m_y0 = 0.0;
    }
    if (m_entranceArmLength > m_exitArmLength && -m_shortHalfAxisB < m_y0) {
        m_z0 = m_longHalfAxisA * sqrt(pow(m_shortHalfAxisB, 2) - pow(m_y0, 2)) /
               m_shortHalfAxisB;
    } else if (m_entranceArmLength < m_exitArmLength &&
               -m_shortHalfAxisB < m_y0) {
        m_z0 = -m_longHalfAxisA *
               sqrt(pow(m_shortHalfAxisB, 2) - pow(m_y0, 2)) / m_shortHalfAxisB;
    } else {
        m_z0 = 0.0;
    }
    if (m_longHalfAxisA > 0.0 && m_y0 < 0.0) {
        mt = pow(m_shortHalfAxisB / m_longHalfAxisA, 2) * m_z0 / m_y0;
    }
    m_tangentAngle = (atan(mt));
    RAYX_D_LOG << "Z0 = " << m_z0 << ", Y0= " << m_y0
             << ", tangentAngle= " << m_tangentAngle;
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

    double angle = atan(tan(theta) * (m_entranceArmLength - m_exitArmLength) /
                        (m_entranceArmLength + m_exitArmLength));
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
    RAYX_D_LOG << "A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB
             << ", C= " << m_halfAxisC
             << ", angle = " << radToDeg(m_tangentAngle);
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

// Null if failed
std::shared_ptr<Ellipsoid> Ellipsoid::createFromXML(const xml::Parser& p) {
    OpticalElement::GeometricalShape geometricalShape = p.parseGeometricalShape();
    double width = p.parseTotalWidth();
    double height = p.parseTotalLength();
    double incidenceAngle = p.parseGrazingIncAngle();
    double entranceArmLength = p.parseEntranceArmLength();
    double exitArmLength = p.parseExitArmLength();
    double azimuthalAngle = p.parseAzimuthalAngle();
    double m_a11 = p.parseParameterA11();
    FigureRotation figRot = p.parseFigureRotation();
    std::array<double, 7> slopeError = p.parseSlopeError();
    Material mat = p.parseMaterial();
    double designGrazing = p.parseDesignGrazingIncAngle();
    double longHalfAxisA = p.parseLongHalfAxisA();
    double shortHalfAxisB = p.parseShortHalfAxisB();
    glm::dvec4 position = p.parsePosition();
    glm::dmat4x4 orientation = p.parseOrientation();

    return std::make_shared<Ellipsoid>(
        p.name(), geometricalShape, width, height, azimuthalAngle, position,
        longHalfAxisA, shortHalfAxisB, designGrazing, orientation,
        incidenceAngle, entranceArmLength, exitArmLength, figRot, m_a11,
        slopeError, mat);
}
}  // namespace RAYX
