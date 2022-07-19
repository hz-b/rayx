#include "PlaneMirror.h"

#include <Data/xml.h>

#include "Debug.h"

namespace RAYX {

/**
 * initializes transformation matrices from position and orientation, and
 * parameters for the quadric in super class (optical element) sets
 * mirror-specific parameters in this class
 * @param name                  name of the plane mirror
 * @param width                 width of mirror (x-dimension in element coord.
 * sys.)
 * @param height                height of mirror (z-dimension in element coord.
 * sys.)
 * @param azimuthalAngle        azimuthal angle of object (rotation in xy plane
 * with respect to previous element) in rad
 * @param position              position in world coordinates (in homogeneous
 * coordinates)
 * @param orientation           orientation in world coordinates
 * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                   material (See Material.h)
 *
 */
PlaneMirror::PlaneMirror(const char* name,
                         OpticalElement::GeometricalShape geometricalShape,
                         const double width, const double height,
                         const double azimuthalAngle, glm::dvec4 position,
                         glm::dmat4x4 orientation,
                         const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;    
    updateObjectParams();

    RAYX_LOG << name;
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, matd, 0}));
}

PlaneMirror::PlaneMirror(const char* name,
                         OpticalElement::GeometricalShape geometricalShape,
                         const double width, const double widthB,
                         const double height, const double azimuthalAngle,
                         glm::dvec4 position, glm::dmat4x4 orientation,
                         const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width, widthB);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;
    updateObjectParams();
    
    RAYX_LOG << name;
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, matd, 0}));
}

std::shared_ptr<PlaneMirror> PlaneMirror::createFromXML(const xml::Parser& p) {
    double widthB;
    bool foundWidthB = xml::paramDouble(p.node, "totalWidthB", &widthB);
    if (foundWidthB) {
        return std::make_shared<PlaneMirror>(
            p.name(), p.parseGeometricalShape(), p.parseTotalWidth(), widthB,
            p.parseTotalLength(), p.parseAzimuthalAngle(), p.parsePosition(),
            p.parseOrientation(), p.parseSlopeError(), p.parseMaterial());
    } else {
        return std::make_shared<PlaneMirror>(
            p.name(), p.parseGeometricalShape(), p.parseTotalWidth(),
            p.parseTotalLength(), p.parseAzimuthalAngle(), p.parsePosition(),
            p.parseOrientation(), p.parseSlopeError(), p.parseMaterial());
    }
}

}  // namespace RAYX
