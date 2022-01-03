#include "Cylinder.h"

#include "Debug.h"

namespace RAYX {
/**
 * @brief Construct a new Cylinder object
 *
 * @param name              Cylinder name
 * @param geometricalShape
 * @param radius            Cylinder radius (if 0, radius auto
 * calculate)
 * @param direction         Direction/Bending radius of Cylinder,
 * (LONG_RADIUS/SHORT_RADIUS)
 * @param width             width of the mirror (x-dimension)
 * @param height            height of the mirror (z-dimension)
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray, in deg
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 */
Cylinder::Cylinder(const char* name,
                   Geometry::GeometricalShape geometricalShape,
                   const double radius, const int direction, const double width,
                   const double height, const double azimuthalAngle,
                   glm::dvec4 position, glm::dmat4x4 orientation,
                   const double grazingIncidence,
                   const double entranceArmLength, const double exitArmLength,
                   const std::vector<double> slopeError, Material mat)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_direction(CYLINDER_DIRECTION(direction)),
      m_radius(radius),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength) {
    RAYX_LOG << name;
    if (m_direction == LONG_RADIUS_R) {  // X-DIR
        m_a11 = 0;
        m_a33 = 1;
        m_a24 = -radius;
    } else {  // Z_DIR/ SHORT_RADIUS_RHO
        m_a11 = 1;
        m_a33 = 0;
        m_a24 = -radius;
    }
    double icurv = 1;
    if (m_a24 > 0) icurv = -1;  // Translated from RAY.FOR
    if (m_radius == 0) {
        setRadius();
    }

    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::vector<double>{
        m_a11, 0, 0, 0, icurv, 1, 0, m_a24, 0, 0, m_a33, 0, 0, 0, matd, 0}));
    // TODO (OS): Add Element parameters?
    setElementParameters({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}
Cylinder::~Cylinder() {}

/**
 * @brief Call when m_radius is 0 and needs to auto-calculate
 *
 */
void Cylinder::setRadius() {
    double theta = radToDeg(m_incidence) * M_PI / 180.0;
    if (m_direction == LONG_RADIUS_R) {
        m_radius = 2.0 / sin(theta) /
                   (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    } else {
        if (m_entranceArmLength == 0.0 || m_exitArmLength == 0.0 ||
            theta == 0.0) {
            m_radius = 0.0;
        } else {
            m_radius = 2.0 * sin(theta) /
                       (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }
    }
}

Cylinder::CYLINDER_DIRECTION Cylinder::getDirection() const {
    return m_direction;
}
double Cylinder::getRadius() const { return m_radius; }
double Cylinder::getIncidenceAngle() const { return m_incidence; }
double Cylinder::getExitArmLength() const { return m_exitArmLength; }
double Cylinder::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX