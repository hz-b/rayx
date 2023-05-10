#pragma once

#include <array>
#include <glm.hpp>

#include "Core.h"
#include "Data/xml.h"
#include "Shared/Constants.h"
#include "utils.h"

namespace RAYX {

enum class GratingMount { Deviation, Incidence };
enum class ImageType { Point2Point, Astigmatic2Astigmatic };

struct RAYX_API OpticalElement2 {
    Element m_element;
    std::string m_name;
};

glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix = true);
glm::dmat4 defaultInMatrix(const DesignObject& dobj);
glm::dmat4 defaultOutMatrix(const DesignObject& dobj);
double defaultMaterial(const DesignObject& dobj);
Rad defaultAzimuthalAngle(const DesignObject& dobj);
Element makeElement(const DesignObject& dobj, Behaviour behaviour, Surface surface);
Surface makeToroid(const DesignObject& dobj);
Surface makeSphere(double radius);
Surface makePlane();
Behaviour makeGrating(const DesignObject& dobj);

}  // namespace RAYX
