#include "PlaneGrating.h"

#include "Debug/Debug.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makePlaneGrating(const DesignElement& dele) {
    auto surface = makePlane();
    auto behaviour = makeGratingEle(dele);
    return makeDesElement(dele, behaviour, surface);
}

}  // namespace RAYX
