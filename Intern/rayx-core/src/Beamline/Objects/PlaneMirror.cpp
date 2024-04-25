#include "PlaneMirror.h"

#include <Data/xml.h>
#include <Shader/Constants.h>
#include <DesignElement/DesignElement.h>

namespace RAYX {

Element makePlaneMirror(const DesignElement& dele) {
    auto behaviour = serializeMirror();
    auto surface = makePlane();
    return makeElement(dele, behaviour, surface);
}

}  // namespace RAYX
