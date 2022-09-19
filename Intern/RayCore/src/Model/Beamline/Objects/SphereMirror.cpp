#include "SphereMirror.h"

#include "Debug.h"

namespace RAYX {

/**
 * Calculates transformation matrices from position and orientation, and sets
 * parameters for the quadric surface. Sets mirror-specific parameters in this
 * class. calculates radius from incidence angle, entrance and exit arm lengths
 *
 * @param name                      name of the optical element
 * @param width                     width of optical element (x dim)
 * @param height                    height of optical element (z dim in element
 * coordinates)
 * @param azimuthalAngle            rotation of element in xy-plane, needed for
 * stokes vector, in rad
 * @param grazingIncidenceAngle     angle in which the main ray should hit the
 * element. given in degree
 * @param position                  position of element in world coordinates
 * @param orientation               orientation of element in world coordinates
 * @param entranceArmLength
 * @param exitArmLength
 * @param slopeError                7 slope error parameters: x-y sagittal (0),
 * y-z meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                       material (See Material.h)
 *
 */
SphereMirror::SphereMirror(const char* name,
                           OpticalElement::GeometricalShape geometricalShape,
                           const double width, const double height,
                           const double azimuthalAngle,
                           const double grazingIncidenceAngle,
                           glm::dvec4 position, glm::dmat4x4 orientation,
                           const double entranceArmLength,
                           const double exitArmLength,
                           const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength),
      m_grazingIncidenceAngle(degToRad(grazingIncidenceAngle))

{
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;

    calcRadius();  // calculate the radius
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        1, 0, 0, 0, 1, 1, 0, -m_radius, 0, 0, 1, 0, 0, 0, matd, 0}));
}

/**
 * Calculates transformation matrices from position and orientation, and sets
 * parameters for the quadric surface. Sets mirror-specific parameters in this
 * class. Radius is not calculated but given as a parameter
 *
 * @param name                      name of the optical element
 * @param width                     width of optical element (x dim)
 * @param height                    height of optical element (z dim in element
 * coordinates)
 * @param azimuthalAngle            rotation of element in xy-plane, needed for
 * stokes vector
 * @param radius                    radius of sphere
 * @param position                  position of element in world coordinates
 * @param orientation               orientation of element in world coordinates
 * @param slopeError                7 slope error parameters: x-y sagittal (0),
 * y-z meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                       material (See Material.h)
 *
 */
SphereMirror::SphereMirror(const char* name,
                           OpticalElement::GeometricalShape geometricalShape,
                           const double width, const double height,
                           const double azimuthalAngle, double radius,
                           glm::dvec4 position, glm::dmat4x4 orientation,
                           const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;

    RAYX_LOG << "Created.";

    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        1, 0, 0, 0, 1, 1, 0, -radius, 0, 0, 1, 0, 0, 0, matd, 0}));
}

std::shared_ptr<SphereMirror> SphereMirror::createFromXML(const xml::Parser& p) {
    return std::make_shared<SphereMirror>(
        p.name(), p.parseGeometricalShape(), p.parseTotalWidth(),
        p.parseTotalLength(), p.parseAzimuthalAngle(), p.parseGrazingIncAngle(),
        p.parsePosition(), p.parseOrientation(), p.parseEntranceArmLength(),
        p.parseExitArmLength(), p.parseSlopeError(), p.parseMaterial());
}

// TODO(Theresa): move this to user params and just give the radius as a
// parameter to the sphere class?
void SphereMirror::calcRadius() {
    m_radius = 2.0 / sin(m_grazingIncidenceAngle) /
               (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
}

double SphereMirror::getRadius() const { return m_radius; }

double SphereMirror::getExitArmLength() const { return m_exitArmLength; }

double SphereMirror::getEntranceArmLength() const {
    return m_entranceArmLength;
}
}  // namespace RAYX
