#include "ImagePlane.h"

#include "Shared/Constants.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) { m_surface = serializePlaneXY(); }

std::array<double, 16> ImagePlane::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
