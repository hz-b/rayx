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

RAYX_API glm::mat4 calcTransformationMatrices(glm::vec4 position, glm::mat4 orientation, bool calcInMatrix = true);
glm::mat4 defaultInMatrix(const DesignObject& dobj);
glm::mat4 defaultOutMatrix(const DesignObject& dobj);
float defaultMaterial(const DesignObject& dobj);
Element makeElement(const DesignObject& dobj, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout = {});
Surface makeToroid(const DesignObject& dobj);
Surface makeSphere(float radius);
Surface makePlane();
Behaviour makeGrating(const DesignObject& dobj);

}  // namespace RAYX
