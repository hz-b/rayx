#include "ImagePlane.h"

#include "Shared/Constants.h"

namespace RAYX {

Element makeImagePlane(const DesignObject& dobj) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXY();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
