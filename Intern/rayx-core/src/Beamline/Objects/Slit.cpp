#include "Slit.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeSlit(const DesignObject& dobj) {
    auto beamstopWidth = dobj.parseTotalWidthStop();
    auto beamstopHeight = dobj.parseTotalHeightStop();
    auto centralBeamstop = dobj.parseCentralBeamstop();

    // TODO the encoding of `centralBeamstop` should not be done by the sign of beamstopWidth/beamstopHeight!
    if (centralBeamstop == CentralBeamstop::None) {
        beamstopWidth = 0;
        beamstopHeight = 0;
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        beamstopWidth = -abs(beamstopWidth);  // negative width expresses that beamstop it elliptical.
        beamstopHeight = abs(beamstopHeight);
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        // Both beamstopWidth and beamstopHeight keep their values.
    }

    Cutout openingCutout;

    auto shape = dobj.parseOpeningShape();
    if (shape == 0) { // rectangle
        openingCutout = serializeRect({
            .m_size_x1 = dobj.parseOpeningWidth(),
            .m_size_x2 = dobj.parseOpeningHeight(),
        });
    } else if (shape == 1) { // elliptical
        openingCutout = serializeElliptical({
            .m_diameter_x1 = dobj.parseOpeningWidth(),
            .m_diameter_x2 = dobj.parseOpeningHeight(),
        });
    } else {
        RAYX_ERR << "unsupported!";
    }

    auto surface = serializePlaneXY();
    auto behaviour = serializeSlit({
        .m_beamstopWidth = beamstopWidth,
        .m_beamstopHeight = beamstopHeight,
        .m_openingCutout = openingCutout,
    });
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
