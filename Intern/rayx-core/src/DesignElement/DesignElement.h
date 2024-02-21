#pragma once

#include "Beamline/Objects/Objects.h"
#include "Beamline/OpticalElement.h"
#include "Shader/Element.h"
#include "Value.h"

namespace RAYX {

struct DesignElement {
    Value v;
    Element compile() const;

    void setName(std::string s);
    std::string getName() const;

    void setWorldPosition(glm::dvec4 p);
    glm::dvec4 getWorldPosition() const;

    void setWorldOrientation(glm::dmat4x4 o);
    glm::dmat4x4 getWorldOrientation() const;

    void setSlopeError(SlopeError s);
    SlopeError getSlopeError() const;

    void setAzimuthalAngle(Rad r);
    Rad getAzimuthalAngle() const;

    void setMaterial(Material m);
    Material getMaterial() const ;
};
}  // namespace RAYX