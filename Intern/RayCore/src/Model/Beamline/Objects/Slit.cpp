#include "Slit.h"

#include "Debug.h"

namespace RAYX {

Slit::Slit(const DesignObject& dobj) : OpticalElement(dobj) {
    auto beamstopWidth = dobj.parseTotalWidthStop();
    auto beamstopHeight = dobj.parseTotalHeightStop();

    m_centralBeamstop = dobj.parseCentralBeamstop();

    m_beamstopWidth = m_centralBeamstop == CentralBeamstop::None
                          ? 0
                          : (m_centralBeamstop == CentralBeamstop::Elliptical ? -abs(beamstopWidth) : abs(beamstopWidth));
    m_beamstopHeight = m_centralBeamstop == CentralBeamstop::None ? 0 : abs(beamstopHeight) != 0;

    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 3, 0, 0, 0}));
}

CentralBeamstop Slit::getCentralBeamstop() const { return m_centralBeamstop; }
double Slit::getBeamstopWidth() const { return m_beamstopWidth; }
double Slit::getBeamstopHeight() const { return m_beamstopHeight; }

glm::dmat4x4 Slit::getElementParameters() const {
    return {m_beamstopWidth / 2, m_beamstopHeight / 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}
}  // namespace RAYX
