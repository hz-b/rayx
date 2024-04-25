#include "Slit.h"

#include <Data/xml.h>
#include <Debug/Debug.h>
#include <Shader/Constants.h>
#include <Shader/Utils.h>
#include <Shader/CutoutFns.h>
#include <DesignElement/DesignElement.h>

namespace RAYX {

Cutout mkOpeningCutout(const DesignElement& dele) {
    auto shape = dele.getOpeningShape();

    // This converts y (height) to z (length), as the RML file uses DesignPlane::XY for slits, whereas our model uses XZ.

    if (shape == CTYPE_RECT) {
        return serializeRect({
            .m_width = dele.getOpeningWidth(),
            .m_length = dele.getOpeningHeight(),
        });
    } else if (shape == CTYPE_ELLIPTICAL) { // elliptical
        return serializeElliptical({
            .m_diameter_x = dele.getOpeningWidth(),
            .m_diameter_z = dele.getOpeningHeight(),
        });
    } else {
        RAYX_ERR << "unsupported opening type!";
        return {};
    }
}

Cutout mkBeamstopCutout(const DesignElement& dele) {
    auto centralBeamstop = dele.getCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        return serializeRect({
            .m_width = 0,
            .m_length = 0,
        });
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        return serializeElliptical({
            .m_diameter_x = dele.getStopWidth(),
            .m_diameter_z = dele.getStopHeight(),
        });
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        return serializeRect({
            .m_width = dele.getStopWidth(),
            .m_length = dele.getStopHeight(),
        });
    } else {
        RAYX_ERR << "unsupported CentralBeamstop type!";
        return {};
    }
}

Element makeSlit(const DesignElement& dele) {
    Cutout openingCutout = mkOpeningCutout(dele);
    Cutout beamstopCutout = mkBeamstopCutout(dele);

    auto surface = serializePlaneXZ();
    auto behaviour = serializeSlit({
        .m_openingCutout = openingCutout,
        .m_beamstopCutout = beamstopCutout,
    });
    Element el = makeElement(dele, behaviour, surface, {}, DesignPlane::XY);
    Cutout globalCutout = el.m_cutout;

    // the opening needs to be a subset of the whole object.
    assertCutoutSubset(openingCutout, globalCutout);

    // the beamstop needs to be a subset of the opening.
    assertCutoutSubset(beamstopCutout, openingCutout);

    return el;
}

}  // namespace RAYX
