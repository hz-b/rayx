#include "Slit.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeSlit(const DesignObject& dobj) {
    auto beamstopWidth = dobj.parseTotalWidthStop();
    auto beamstopHeight = dobj.parseTotalHeightStop();
    auto centralBeamstop = dobj.parseCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        beamstopWidth = 0;
        beamstopHeight = 0;
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        beamstopWidth = -abs(beamstopWidth);  // negative width expresses that beamstop it elliptical.
        beamstopHeight = abs(beamstopHeight);
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        // Both beamstopWidth and beamstopHeight keep their values.
    }

    auto surface = serializePlaneXY();
    auto behaviour = serializeSlit({
        .m_beamstopWidth = beamstopWidth,
        .m_beamstopHeight = beamstopHeight,
        .m_gapCutout = dobj.parseCutout(),
    });
    auto el = makeElement(dobj, behaviour, surface);
    el.m_cutout = serializeUnlimited();
    return el;
}

}  // namespace RAYX
