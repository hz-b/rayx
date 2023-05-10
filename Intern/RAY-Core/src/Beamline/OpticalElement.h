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

class RAYX_API OpticalElement {
  public:
    OpticalElement(const DesignObject&);

    /// Converts `this` into an Element.
    Element intoElement() const;

    virtual ~OpticalElement() = default;

    glm::dmat4 getInMatrix() const;
    glm::dmat4 getOutMatrix() const;
    glm::dmat4x4 getOrientation() const;
    glm::dvec4 getPosition() const;

    std::array<double, 7> getSlopeError() const;

    std::string m_name;
    Material m_material;

  protected:
    Rad m_azimuthalAngle = Rad(0);                // rotation of element through xy-plane
                                                  // (needed for stokes vector)
    glm::dmat4x4 m_orientation = glm::dmat4x4();  //< Orientation matrix of element (is basis)
    glm::dvec4 m_position = glm::dvec4();         //< Position of element in world coordinates

    std::array<double, 7> m_slopeError;

    Behaviour m_behaviour;
    Surface m_surface;
    Cutout m_cutout;
};

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
