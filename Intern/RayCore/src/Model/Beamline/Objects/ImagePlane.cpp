#include "ImagePlane.h"

#include "Constants.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surfaceType = STYPE_PLANE_XY;
    m_surfaceParams = {0.0};
}

std::array<double, 16> ImagePlane::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
