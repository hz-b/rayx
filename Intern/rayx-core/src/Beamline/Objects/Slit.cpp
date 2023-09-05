#include "Slit.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Cutout mkOpeningCutout(const DesignObject& dobj) {
    auto shape = dobj.parseOpeningShape();
    if (shape == 0) { // rectangle
        return serializeRect({
            .m_size_x1 = dobj.parseOpeningWidth(),
            .m_size_x2 = dobj.parseOpeningHeight(),
        });
    } else if (shape == 1) { // elliptical
        return serializeElliptical({
            .m_diameter_x1 = dobj.parseOpeningWidth(),
            .m_diameter_x2 = dobj.parseOpeningHeight(),
        });
    } else {
        RAYX_ERR << "unsupported!";
        return {};
    }
}

Cutout mkBeamstopCutout(const DesignObject& dobj) {
    auto centralBeamstop = dobj.parseCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        return serializeRect({
            .m_size_x1 = 0,
            .m_size_x2 = 0,
        });
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        return serializeElliptical({
            .m_diameter_x1 = dobj.parseTotalWidthStop(),
            .m_diameter_x2 = dobj.parseTotalHeightStop(),
        });
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        return serializeRect({
            .m_size_x1 = dobj.parseTotalWidthStop(),
            .m_size_x2 = dobj.parseTotalHeightStop(),
        });
    } else {
        RAYX_ERR << "unsupported CentralBeamstop type!";
        return {};
    }
}

Element makeSlit(const DesignObject& dobj) {
    auto surface = serializePlaneXY();
    auto behaviour = serializeSlit({
        .m_openingCutout = mkOpeningCutout(dobj),
        .m_beamstopCutout = mkBeamstopCutout(dobj),
    });
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
