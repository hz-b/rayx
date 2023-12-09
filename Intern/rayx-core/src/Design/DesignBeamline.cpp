#include "DesignBeamline.h"
#include "Debug/Debug.h"
#include "Strings.h"

namespace RAYX {

Beamline DesignBeamline::compile() const {
    for (const auto& el : field<ElementStr>()) {
        RAYX_LOG << el.field<NameStr>();
        const DesignCutout& c = el.field<CutoutStr>();
        if (c.is<RectStr>()) {
            const DesignRect& r = c.as<RectStr>();
            RAYX_LOG << "length=" << r.field<LengthStr>();
        }
    }

    for (const auto& el : field<SourceStr>()) {
        RAYX_LOG << "numberOfRays=" << el.field<NumberOfRaysStr>();
    }

    return {};
}

}
