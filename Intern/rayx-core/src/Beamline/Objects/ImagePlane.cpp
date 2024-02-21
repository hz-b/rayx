#include "ImagePlane.h"

#include "Shader/Constants.h"

namespace RAYX {

Element makeImagePlane(const DesignElement& dele) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXZ();
    auto cutout = serializeUnlimited();
    return makeDesElement(dele, behaviour, surface, cutout, DesignPlane::XY);
}

}  // namespace RAYX
