#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane() = default;
    ImagePlane(const DesignObject&);
    ~ImagePlane() = default;

    double getDistance();
};
}  // namespace RAYX
