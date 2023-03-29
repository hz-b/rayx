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

    void calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, glm::dmat4& output, bool calcInMatrix = true) const;

    glm::dmat4 getInMatrix() const;
    glm::dmat4 getOutMatrix() const;
    glm::dmat4x4 getOrientation() const;
    glm::dvec4 getPosition() const;

    virtual std::array<double, 16> getBehaviourParams() const;
    virtual int getBehaviourType() const = 0;
    std::array<double, 7> getSlopeError() const;

    std::string m_name;
    Material m_material;

  protected:
    Rad m_azimuthalAngle = Rad(0);                // rotation of element through xy-plane
                                                  // (needed for stokes vector)
    glm::dmat4x4 m_orientation = glm::dmat4x4();  //< Orientation matrix of element (is basis)
    glm::dvec4 m_position = glm::dvec4();         //< Position of element in world coordinates

    std::array<double, 7> m_slopeError;
    Surface m_surface;
    Cutout m_cutout;
};

}  // namespace RAYX
