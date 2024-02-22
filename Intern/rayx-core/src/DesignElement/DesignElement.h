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


    void setProfileKind(int value);
    int getProfileKind() const;

    void setProfileFile(double value);
    double getProfileFile() const;


    void setTotalLength(double value);
    double getTotalLength() const;

    void setGrazingIncAngle(Rad value);
    Rad getGrazingIncAngle() const;

    void setEntranceArmLength(double value);
    double getEntranceArmLength() const;

    void setExitArmLength(double value);
    double getExitArmLength() const;

    void setRadiusDirection(CylinderDirection value);
    CylinderDirection getRadiusDirection() const;

    // Setter and Getter for radius
    void setRadius(double value);
    double getRadius() const;

    void setDesignGrazingIncAngle(Rad value);
    Rad getDesignGrazingIncAngle() const;

    // Setter and Getter for longHalfAxisA
    void setLongHalfAxisA(double value);
    double getLongHalfAxisA() const;

    // Setter and Getter for shortHalfAxisB
    void setShortHalfAxisB(double value);
    double getShortHalfAxisB() const;

    void setParameterA11(double value);
    double getParameterA11() const;

    // Setter and Getter for figureRotation
    void setFigureRotation(FigureRotation value);
    FigureRotation getFigureRotation() const;

    void setArmLength(double value);
    double getArmLength() const;

    void setParameterP(double value);
    double getParameterP() const;

    void setParameterPType(double value);
    double getParameterPType() const;
};
}  // namespace RAYX