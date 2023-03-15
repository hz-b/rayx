#include "Slit.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
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

    m_gapWidthA = m_excerptParams[0];
    m_gapWidthB = m_excerptParams[1];
    m_gapHeight = m_excerptParams[2];

    m_excerptParams[0] = 0;
    m_excerptParams[1] = 0;
    m_excerptParams[2] = 0;

    m_excerptType = XTY_UNLIMITED;

    m_surfaceType = STY_INF_PLANE;
    m_surfaceParams = {0.0};
}

CentralBeamstop Slit::getCentralBeamstop() const { return m_centralBeamstop; }
double Slit::getBeamstopWidth() const { return m_beamstopWidth; }
double Slit::getBeamstopHeight() const { return m_beamstopHeight; }

std::array<double, 16> Slit::getElementParams() const {
    return {m_beamstopWidth / 2, m_beamstopHeight / 2, m_gapWidthA, m_gapWidthB, m_gapHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}
}  // namespace RAYX
