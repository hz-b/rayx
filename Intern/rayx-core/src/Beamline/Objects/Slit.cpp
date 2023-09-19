#include "Slit.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Cutout mkOpeningCutout(const DesignObject& dobj) {
    auto shape = dobj.parseOpeningShape();

    // This converts y (height) to z (length), as the RML file uses DesignPlane::XY for slits, whereas our model uses XZ.

    if (shape == CTYPE_RECT) {
        return serializeRect({
            .m_width = dobj.parseOpeningWidth(),
            .m_length = dobj.parseOpeningHeight(),
        });
    } else if (shape == CTYPE_ELLIPTICAL) { // elliptical
        return serializeElliptical({
            .m_diameter_x = dobj.parseOpeningWidth(),
            .m_diameter_z = dobj.parseOpeningHeight(),
        });
    } else {
        RAYX_ERR << "unsupported opening type!";
        return {};
    }
}

Cutout mkBeamstopCutout(const DesignObject& dobj) {
    auto centralBeamstop = dobj.parseCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        return serializeRect({
            .m_width = 0,
            .m_length = 0,
        });
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        return serializeElliptical({
            .m_diameter_x = dobj.parseTotalWidthStop(),
            .m_diameter_z = dobj.parseTotalHeightStop(),
        });
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        return serializeRect({
            .m_width = dobj.parseTotalWidthStop(),
            .m_length = dobj.parseTotalHeightStop(),
        });
    } else {
        RAYX_ERR << "unsupported CentralBeamstop type!";
        return {};
    }
}

Element makeSlit(const DesignObject& dobj) {
    auto surface = serializePlaneXZ();
    auto behaviour = serializeSlit({
        .m_openingCutout = mkOpeningCutout(dobj),
        .m_beamstopCutout = mkBeamstopCutout(dobj),
    });
    return makeElement(dobj, behaviour, surface, {}, DesignPlane::XY);
}

}  // namespace RAYX
