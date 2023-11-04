#include "ImagePlane.h"

#include "Shader/Constants.h"

namespace RAYX {

Element makeImagePlane(const DesignObject& dobj) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXZ();
    auto cutout = serializeUnlimited();
    return makeElement(dobj, behaviour, surface, cutout, DesignPlane::XY);
}

}  // namespace RAYX
