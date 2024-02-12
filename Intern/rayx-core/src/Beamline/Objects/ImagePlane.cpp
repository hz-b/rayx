#include "ImagePlane.h"

#include "Shader/Constants.h"

namespace RAYX {

Element makeImagePlane(const DesignElement& dobj) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXZ();
    auto cutout = serializeUnlimited();
    return makeDesElement(dobj, behaviour, surface, cutout, DesignPlane::XY);
}

}  // namespace RAYX
