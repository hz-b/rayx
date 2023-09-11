#include "ImagePlane.h"

#include "Shared/Constants.h"

namespace RAYX {

Element makeImagePlane(const DesignObject& dobj) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXY();
    auto cutout = serializeUnlimited();
    return makeElement(dobj, behaviour, surface, cutout);
}

}  // namespace RAYX
