#include "PlaneGrating.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makePlaneGrating(DesignObject& dobj) {
    auto vls = dobj.parseVls();
    auto surface = makePlane();
    auto behaviour = serializeGrating({
        .m_vls = {vls[0], vls[1], vls[2], vls[3], vls[4], vls[5]},
        .m_lineDensity = dobj.parseLineDensity(),
        .m_orderOfDiffraction = dobj.parseOrderDiffraction(),
    });
    return defaultElement(dobj, behaviour, surface);
}

}  // namespace RAYX
