#include "Slit.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"
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

    m_surface = serializePlaneXY();

    m_gapCutout = m_cutout;
    m_cutout = serializeUnlimited();

    m_behaviour = serializeSlit({
        .m_beamstopWidth = m_beamstopWidth,
        .m_beamstopHeight = m_beamstopHeight,
        .m_gapCutout = m_gapCutout,
    });
}

CentralBeamstop Slit::getCentralBeamstop() const { return m_centralBeamstop; }
double Slit::getBeamstopWidth() const { return m_beamstopWidth; }
double Slit::getBeamstopHeight() const { return m_beamstopHeight; }

}  // namespace RAYX
