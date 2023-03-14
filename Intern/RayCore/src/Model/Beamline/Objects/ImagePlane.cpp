#include "ImagePlane.h"

#include "Constants.h"
#include "Model/Surface/InfPlane.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) { setSurface(std::make_unique<InfPlane>()); }

glm::dmat4x4 ImagePlane::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
