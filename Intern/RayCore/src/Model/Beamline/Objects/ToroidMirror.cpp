#include "ToroidMirror.h"

#include "Debug.h"

namespace RAYX {

/**
 * Initializes transformation matrices, and parameters for the toroid in super
 * class (optical element). Sets mirror-specific parameters in this class.
 *
 * @param name                      name of the element
 * @param geometricalShape          either rectangular or elliptical
 * @param width                     width of the element
 * @param height                    height of the element
 * @param azimuthalAngle            rotation of element in xy-plane, needed for
 * stokes vector, in rad
 * @param grazingIncidenceAngle     angle in which the main ray should hit the
 * element, used to calculate the radii, in rad
 * @param position                  position of element in world coordinate
 * system
 * @param orientation               orientation of element in world coordinate
 * system
 * @param longRadius                curvature parameter: radius in z-y-plane
 * @param shortRadius               curvature parameter: radius in x-y-plane
 * @param slopeError                7 slope error parameters: x-y sagittal (0),
 * y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                       material (See Material.h)
 */
ToroidMirror::ToroidMirror(const char* name,
                           Geometry::GeometricalShape geometricalShape,
                           const double width, const double height,
                           const double azimuthalAngle, glm::dvec4 position,
                           glm::dmat4x4 orientation,
                           const double longRadius, const double shortRadius,
                           const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_longRadius(longRadius),
      m_shortRadius(shortRadius) {
    // TODO(Theresa): maybe move this function outside of this class (same for
    // spheres) because this can be derived from user parameters

    RAYX_LOG << "long Radius: " << m_longRadius
             << ", short Radius: " << m_shortRadius;
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Toroid>(
        std::array<double, 4 * 4>{m_longRadius, m_shortRadius, 0, 0, 0, 1, 0, 0,
                                  0, 0, 1, 0, 6, 0, matd, 0}));
    // setSurface(std::make_unique<Toroid>(m_longRadius, m_shortRadius));
}

ToroidMirror::~ToroidMirror() {}

std::shared_ptr<ToroidMirror> ToroidMirror::createFromXML(xml::Parser p) {
    return std::make_shared<ToroidMirror>(
        p.name(), p.parseGeometricalShape(), p.parseTotalLength(),
        p.parseTotalWidth(), p.parseAzimuthalAngle(), p.parsePosition(),
        p.parseOrientation(), p.parseLongRadius(),
        p.parseShortRadius(), p.parseSlopeError(), p.parseMaterial());
}

}  // namespace RAYX
