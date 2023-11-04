#include "SphereMirror.h"

#include "Beamline/OpticalElement.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeSphereMirror(const DesignObject& dobj) {
    auto entranceArmLength = dobj.parseEntranceArmLength();
    auto exitArmLength = dobj.parseExitArmLength();
    auto grazingIncidenceAngle = dobj.parseGrazingIncAngle();
    auto radius = 2.0 / grazingIncidenceAngle.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);

    auto behaviour = serializeMirror();
    auto surface = makeSphere(radius);
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
