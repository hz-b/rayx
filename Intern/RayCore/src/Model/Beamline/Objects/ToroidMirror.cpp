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
 * @param mEntrance                 meridional entrance arm length, used to
 * calculate the radii
 * @param mExit                     meridional exit arm length, used to
 * calculate the radii
 * @param sEntrance                 sagittal entrance arm length, used to
 * calculate the radii
 * @param sExit                     sagittal exit arm length, used to calculate
 * the radii
 * @param slopeError                7 slope error parameters: x-y sagittal (0),
 * y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                       material (See Material.h)
 */
ToroidMirror::ToroidMirror(
    const char* name, Geometry::GeometricalShape geometricalShape,
    const double width, const double height, const double azimuthalAngle,
    glm::dvec4 position, glm::dmat4x4 orientation, const double incidenceAngle,
    const double mEntrance, const double mExit, const double sEntrance,
    const double sExit, const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     geometricalShape, width, height, azimuthalAngle, position,
                     orientation, slopeError),
      m_sagittalEntranceArmLength(sEntrance),
      m_sagittalExitArmLength(sExit),
      m_meridionalEntranceArmLength(mEntrance),
      m_meridionalExitArmLength(mExit) {
    // TODO(Theresa): maybe move this function outside of this class (same for
    // spheres) because this can be derived from user parameters
    calcRadius(incidenceAngle);  // calculate the radius

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
        p.parseOrientation(), p.parseGrazingIncAngle(),
        p.parseEntranceArmLengthMer(), p.parseExitArmLengthMer(),
        p.parseEntranceArmLengthSag(), p.parseExitArmLengthSag(),
        p.parseSlopeError(), p.parseMaterial());
}

/**
 * calculate long and short radius from grazing incidence angle and meridional
 * and sagittal entrance and exit arm lengths
 */
void ToroidMirror::calcRadius(double incidenceAngle) {
    m_longRadius =
        2.0 / sin(incidenceAngle) /
        (1.0 / m_meridionalEntranceArmLength + 1.0 / m_meridionalExitArmLength);

    if (m_meridionalEntranceArmLength == 0.0 ||
        m_meridionalExitArmLength == 0.0 || incidenceAngle == 0.0) {
        m_shortRadius = 0.0;
    } else {
        m_shortRadius =
            2.0 * sin(incidenceAngle) /
            (1.0 / m_sagittalEntranceArmLength + 1.0 / m_sagittalExitArmLength);
    }
}

double ToroidMirror::getSagittalEntranceArmLength() const {
    return m_sagittalEntranceArmLength;
}

double ToroidMirror::getSagittalExitArmLength() const {
    return m_sagittalExitArmLength;
}

double ToroidMirror::getMeridionalEntranceArmLength() const {
    return m_meridionalEntranceArmLength;
}

double ToroidMirror::getMeridionalExitArmLength() const {
    return m_meridionalExitArmLength;
}
}  // namespace RAYX
