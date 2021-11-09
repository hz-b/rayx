#pragma once
#include "Model/Surface/Quadric.h"
#include "Model/Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API PlaneMirror : public OpticalElement {

    public:

        PlaneMirror(const char* name, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);
        PlaneMirror();
        ~PlaneMirror();

        static std::shared_ptr<PlaneMirror> createFromXML(rapidxml::xml_node<>*);

    };

} // namespace RAYX