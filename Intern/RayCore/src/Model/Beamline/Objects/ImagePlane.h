#pragma once
#include <Data/xml.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane(const char* name, glm::dvec4 position, glm::dmat4x4 orientation);
    ImagePlane();
    ~ImagePlane();

    static std::shared_ptr<ImagePlane> createFromXML(xml::Parser);

    double getDistance();
};
}  // namespace RAYX
