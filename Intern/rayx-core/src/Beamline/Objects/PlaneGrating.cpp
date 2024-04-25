#include "PlaneGrating.h"

#include <Debug/Debug.h>
#include <Shader/Constants.h>
#include <DesignElement/DesignElement.h>

namespace RAYX {

Element makePlaneGrating(const DesignElement& dele) {
    auto surface = makePlane();
    auto behaviour = makeGrating(dele);
    return makeElement(dele, behaviour, surface);
}

}  // namespace RAYX
