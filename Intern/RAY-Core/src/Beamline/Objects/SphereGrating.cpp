#include "SphereGrating.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeSphereGrating(const DesignObject& dobj) {
    // auto designEnergyMounting = dobj.parseDesignEnergy();
    auto lineDensity = dobj.parseLineDensity();
    auto orderOfDiffraction = dobj.parseOrderDiffraction();
    auto vls = dobj.parseVls();

    // auto gratingMount = dobj.parseGratingMount();
    auto radius = dobj.parseRadius();
    auto surface = makeSphere(radius);

    auto behaviour = serializeGrating({
        .m_vls = {vls[0], vls[1], vls[2], vls[3], vls[4], vls[5]},
        .m_lineDensity = lineDensity,
        .m_orderOfDiffraction = orderOfDiffraction,
    });

    return defaultElement(dobj, behaviour, surface);
}

}  // namespace RAYX
