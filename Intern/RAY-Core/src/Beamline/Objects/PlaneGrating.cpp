#include "PlaneGrating.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makePlaneGrating(DesignObject& dobj) {
    auto surface = makePlane();
    auto behaviour = makeGrating(dobj);
    return defaultElement(dobj, behaviour, surface);
}

}  // namespace RAYX
