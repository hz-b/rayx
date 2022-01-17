#pragma once
#include <Data/xml.h>
#include <Tracer/Vulkan/Material.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API PlaneMirror : public OpticalElement {
  public:
    PlaneMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                const double width, const double height,
                const double azimuthalAngle, glm::dvec4 position,
                glm::dmat4x4 orientation,
                const std::array<double, 7> slopeError, Material mat);
    PlaneMirror();
    ~PlaneMirror();

    static std::shared_ptr<PlaneMirror> createFromXML(
        rapidxml::xml_node<>*, const std::vector<xml::Group>& group_context);
};

}  // namespace RAYX