#pragma once

#include <array>
#include <glm.hpp>
#include <optional>

#include "Core.h"
#include "Data/xml.h"
#include "Shared/Constants.h"
#include "utils.h"

namespace RAYX {

enum class GratingMount { Deviation, Incidence };

struct RAYX_API OpticalElement {
    Element m_element;
    std::string m_name;
};

RAYX_API glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix, DesignPlane plane);
glm::dmat4 defaultInMatrix(const DesignObject& dobj, DesignPlane plane);
glm::dmat4 defaultOutMatrix(const DesignObject& dobj, DesignPlane plane);
double defaultMaterial(const DesignObject& dobj);
Element makeElement(const DesignObject& dobj, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout = {}, DesignPlane plane=DesignPlane::XZ);
Surface makeToroid(const DesignObject& dobj);
Surface makeSphere(double radius);
Surface makePlane();
Behaviour makeGrating(const DesignObject& dobj);

}  // namespace RAYX
