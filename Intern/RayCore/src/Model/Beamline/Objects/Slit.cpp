#include "Slit.h"

#include "Debug.h"

namespace RAYX {

/**
 * angles given in degree and stored in rad
 * initializes transformation matrices, and parameters for the quadric in super
 * class (quadric) sets mirror-specific parameters in this class
 * @param name                  name of optical element
 * @param geometricalShape      0/1 rectangle or elliptical
 * @param beamstop              0/1 whether there is a central beamstop
 * @param width                 width of mirror (x-dimension in element coord.
 * sys.)
 * @param height                height of mirror (z-dimension in element coord.
 * sys.)
 * @param position              position in world coordinates
 * @param orientation           orientation in world coordinates
 * @param beamstopWidth         width of central beamstop
 * @param beamstopHeight        height of central beamstop
 * @param sourceEnergy          energy of source
 *
 */
Slit::Slit(const char* name, GeometricalShape geometricalShape, CentralBeamstop beamstop, double width, double height, glm::dvec4 position,
           glm::dmat4x4 orientation, double beamstopWidth, double beamstopHeight)
    : OpticalElement(name, {0, 0, 0, 0, 0, 0, 0})  // no azimuthal angle for slit bc no efficiency needed
{
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;
    m_centralBeamstop = beamstop;

    // if no beamstop -> set to zero
    // if elliptical set width (xStop) to negative value to encode the shape
    // (xStop < 0 -> Elliptical, xStop > 0 -> rectangle, xStop = yStop = 0 ->
    // none)
    m_beamstopWidth = m_centralBeamstop == CentralBeamstop::None
                          ? 0
                          : (m_centralBeamstop == CentralBeamstop::Elliptical ? -abs(beamstopWidth) : abs(beamstopWidth));
    m_beamstopHeight = m_centralBeamstop == CentralBeamstop::None ? 0 : abs(beamstopHeight) != 0;

    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 3, 0, 0, 0}));
}

std::shared_ptr<Slit> Slit::createFromXML(const xml::Parser& p) {
    return std::make_shared<Slit>(p.name(), p.parseGeometricalShape(), p.parseCentralBeamstop(), p.parseTotalWidth(), p.parseTotalHeight(),
                                  p.parsePosition(), p.parseOrientation(), p.parseTotalWidthStop(), p.parseTotalHeightStop());
}

CentralBeamstop Slit::getCentralBeamstop() const { return m_centralBeamstop; }
double Slit::getBeamstopWidth() const { return m_beamstopWidth; }
double Slit::getBeamstopHeight() const { return m_beamstopHeight; }

glm::dmat4x4 Slit::getElementParameters() const {
    return {m_beamstopWidth / 2, m_beamstopHeight / 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}
}  // namespace RAYX
