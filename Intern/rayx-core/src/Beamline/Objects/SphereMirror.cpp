#include "SphereMirror.h"

#include <Beamline/OpticalElement.h>
#include <Data/xml.h>
#include <Debug/Debug.h>
#include <Material/Material.h>
#include <Shader/Constants.h>
#include <DesignElement/DesignElement.h>

namespace RAYX {

Element makeSphereMirror(const DesignElement& dele) {
    auto entranceArmLength = dele.getEntranceArmLength();
    auto exitArmLength = dele.getExitArmLength();
    auto grazingIncidenceAngle = dele.getGrazingIncAngle();
    auto radius = 2.0 / grazingIncidenceAngle.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);

    auto behaviour = serializeMirror();
    auto surface = makeSphere(radius);
    return makeElement(dele, behaviour, surface);
}

}  // namespace RAYX
