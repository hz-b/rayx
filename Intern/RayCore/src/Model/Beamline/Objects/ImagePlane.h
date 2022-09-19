#pragma once
#include <Data/xml.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API ImagePlane : public OpticalElement {
  public:
    ImagePlane() = default;
    ImagePlane(const char* name, glm::dvec4 position, glm::dmat4x4 orientation);
    ~ImagePlane();

    static std::shared_ptr<ImagePlane> createFromXML(const xml::Parser&);

    double getDistance();
    std::array<double, 4*4> getElementParameters() const;
};
}  // namespace RAYX
