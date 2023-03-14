#include "ImagePlane.h"

#include "Constants.h"
#include "Model/Surface/InfPlane.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) { setSurface(std::make_unique<InfPlane>()); }

std::array<double, 16> ImagePlane::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
