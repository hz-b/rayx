#include "Slit.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Model/Surface/InfPlane.h"

namespace RAYX {

Slit::Slit(const DesignObject& dobj) : OpticalElement(dobj) {
    auto beamstopWidth = dobj.parseTotalWidthStop();
    auto beamstopHeight = dobj.parseTotalHeightStop();

    m_centralBeamstop = dobj.parseCentralBeamstop();

    if (m_centralBeamstop == CentralBeamstop::None) {
        m_beamstopWidth = 0;
        m_beamstopHeight = 0;
    } else if (m_centralBeamstop == CentralBeamstop::Elliptical) {
        m_beamstopWidth = -abs(beamstopWidth);  // negative width expresses that beamstop it elliptical.
        m_beamstopHeight = abs(beamstopHeight);
    } else if (m_centralBeamstop == CentralBeamstop::Rectangle) {
        m_beamstopWidth = beamstopWidth;
        m_beamstopHeight = beamstopHeight;
    }

    setSurface(std::make_unique<InfPlane>());
}

CentralBeamstop Slit::getCentralBeamstop() const { return m_centralBeamstop; }
double Slit::getBeamstopWidth() const { return m_beamstopWidth; }
double Slit::getBeamstopHeight() const { return m_beamstopHeight; }

std::array<double, 16> Slit::getElementParams() const {
    return {m_beamstopWidth / 2, m_beamstopHeight / 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}
}  // namespace RAYX
