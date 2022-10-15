#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API PlaneMirror : public OpticalElement {
  public:
    PlaneMirror(const char* name, GeometricalShape geometricalShape, const double width, const double height, const double azimuthalAngle,
                glm::dvec4 position, glm::dmat4x4 orientation, const std::array<double, 7> slopeError, Material mat);
    PlaneMirror(const char* name, GeometricalShape geometricalShape, const double width, const double widthB, const double height,
                const double azimuthalAngle, glm::dvec4 position, glm::dmat4x4 orientation, const std::array<double, 7> slopeError,
                Material mat);

    static std::shared_ptr<PlaneMirror> createFromXML(const xml::Parser&);
};

}  // namespace RAYX