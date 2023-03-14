#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane() = default;
    ImagePlane(const DesignObject&);
    ~ImagePlane() = default;

    inline int getElementType() const { return TY_IMAGE_PLANE; }

    double getDistance();
    std::array<double, 16> getElementParams() const;
};
}  // namespace RAYX
