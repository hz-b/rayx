#include "ImagePlane.h"

#include <Shader/Constants.h>
#include <DesignElement/DesignElement.h>

namespace RAYX {

Element makeImagePlane(const DesignElement& dele) {
    auto behaviour = serializeImagePlane();
    auto surface = serializePlaneXZ();
    auto cutout = serializeUnlimited();
    return makeElement(dele, behaviour, surface, cutout, DesignPlane::XY);
}

}  // namespace RAYX
