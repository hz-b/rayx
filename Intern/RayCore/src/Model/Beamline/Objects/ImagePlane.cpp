#include "ImagePlane.h"

#include "Shared/Constants.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surface = serializePlaneXY();
    m_behaviour = serializeImagePlane();
}

}  // namespace RAYX
