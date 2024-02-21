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
 
    void setMisalignment(Misalignment m);
    Misalignment getMisalignment() const;   

    void setSlopeError(SlopeError s);
    SlopeError getSlopeError() const;

    void setAzimuthalAngle(Rad r);
    Rad getAzimuthalAngle() const;

    void setMaterial(Material m);
    Material getMaterial() const;

    void setCutout(Cutout c);
    Cutout getCutout() const;

    void setDistancePreceding(double d);
    double getDistancePreceding() const;

    void setTotalHeight(double d);
    double getTotalHeight() const;

    void setOpeningShape(double shape);
    double getOpeningShape() const;

    void setOpeningWidth(double d);
    double getOpeningWidth() const;

    void setOpeningHeight(double d);
    double getOpeningHeight() const;

    void setCentralBeamstop(CentralBeamstop d);
    CentralBeamstop getCentralBeamstop() const;

    void setStopWidth(double d);
    double getStopWidth() const;

    void setStopHeight(double height);
    double getStopHeight() const;

    void setTotalWidth(double width);
    double getTotalWidth() const;

};
}  // namespace RAYX