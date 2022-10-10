#include "Cylinder.h"

#include <ext/scalar_constants.hpp>

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
 * @param azimuthalAngle        azimuthal angle of object (rotation in xy plane
 * with respect to previous element) in rad
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray, in deg
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat               material (See Material.h)
 *
 */
Cylinder::Cylinder(const char* name, OpticalElement::GeometricalShape geometricalShape, const double radius, CylinderDirection direction,
                   const double width, const double height, const double azimuthalAngle, glm::dvec4 position, glm::dmat4x4 orientation,
                   const double grazingIncidence, const double entranceArmLength, const double exitArmLength, const std::array<double, 7> slopeError,
                   Material mat)
    : OpticalElement(name, slopeError),
      m_direction(direction),
      m_radius(radius),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;

    RAYX_VERB << ((m_direction == CylinderDirection::LongRadiusR) ? "LONG RADIUS" : "SHORT RADIUS");
    if (m_direction == CylinderDirection::LongRadiusR) {  // X-DIR
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

    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{m_a11, 0, 0, 0, icurv, 1, 0, m_a24, 0, 0, m_a33, 0, 0, 0, matd, 0}));
}
Cylinder::~Cylinder() = default;

/**
 * @brief Call when m_radius is 0 and needs to auto-calculate
 *
 */
void Cylinder::setRadius() {
    double theta = radToDeg(m_incidence) * glm::pi<double>() / 180.0;
    if (m_direction == CylinderDirection::LongRadiusR) {
        m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    } else {
        if (m_entranceArmLength == 0.0 || m_exitArmLength == 0.0 || theta == 0.0) {
            m_radius = 0.0;
        } else {
            m_radius = 2.0 * sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }
    }

    RAYX_VERB << "Radius: " << m_radius;
}

std::shared_ptr<Cylinder> Cylinder::createFromXML(const xml::Parser& p) {
    return std::make_shared<Cylinder>(p.name(), p.parseGeometricalShape(), p.parseRadius(), p.parseBendingRadius(), p.parseTotalWidth(),
                                      p.parseTotalLength(), p.parseAzimuthalAngle(), p.parsePosition(), p.parseOrientation(),
                                      p.parseGrazingIncAngle(), p.parseEntranceArmLength(), p.parseExitArmLength(), p.parseSlopeError(),
                                      p.parseMaterial());
}

CylinderDirection Cylinder::getDirection() const { return m_direction; }
double Cylinder::getRadius() const { return m_radius; }
double Cylinder::getIncidenceAngle() const { return m_incidence; }
double Cylinder::getExitArmLength() const { return m_exitArmLength; }
double Cylinder::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
