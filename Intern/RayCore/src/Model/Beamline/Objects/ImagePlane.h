#pragma once
#include <Data/xml.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane() = default;
    ImagePlane(const DesignObject&);
    ~ImagePlane() = default;

    double getDistance();
    glm::dmat4x4 getElementParameters() const;
};
}  // namespace RAYX
