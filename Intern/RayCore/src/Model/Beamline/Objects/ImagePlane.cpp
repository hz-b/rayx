#include "ImagePlane.h"

#include "Model/Surface/Quadric.h"

namespace RAYX {

ImagePlane::ImagePlane(const DesignObject& dobj) : OpticalElement(dobj) {
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0}));
}

glm::dmat4x4 ImagePlane::getElementParameters() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
