#include "PlaneMirror.h"

#include "Data/xml.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makePlaneMirror(const DesignElement& dele) {
    auto behaviour = serializeMirror();
    auto surface = makePlane();
    return makeDesElement(dele, behaviour, surface);
}

}  // namespace RAYX
