#pragma once
#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"
#include <Data/xml.h>
#include <vector>

namespace RAYX {

class RAYX_API PlaneMirror : public OpticalElement {
  public:
    PlaneMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                const double width, const double height, glm::dvec4 position,
                glm::dmat4x4 orientation, const std::vector<double> slopeError);
    PlaneMirror();
    ~PlaneMirror();

    static std::shared_ptr<PlaneMirror> createFromXML(
        rapidxml::xml_node<>*, const std::vector<xml::Group>& group_context);
};

}  // namespace RAYX