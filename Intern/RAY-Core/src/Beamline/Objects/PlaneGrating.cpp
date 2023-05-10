#include "PlaneGrating.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makePlaneGrating(DesignObject& dobj) {
    auto surface = makePlane();
    auto behaviour = makeGrating(dobj);
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
