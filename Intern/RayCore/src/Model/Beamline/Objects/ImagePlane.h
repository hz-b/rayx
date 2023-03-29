#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane() = default;
    ImagePlane(const DesignObject&);
    ~ImagePlane() = default;

    inline int getBehaviourType() const { return BTYPE_IMAGE_PLANE; }

    double getDistance();
    std::array<double, 16> getBehaviourParams() const;
};
}  // namespace RAYX
