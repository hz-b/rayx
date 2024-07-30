#pragma once

#include "Element/Element.h"
#include "Value.h"

namespace RAYX {

struct RAYX_API DesignElement {
    DesignMap m_elementParameters;
    Element compile() const;

    void setName(std::string s);
    void setType(ElementType s);

    std::string getName() const;
    ElementType getType() const;

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
    Cutout getGlobalCutout() const;

    void setVLSParameters(const std::array<double, 6>& values);
    std::array<double, 6> getVLSParameters() const;

    void setExpertsOptics(Surface value);
    Surface getExpertsOptics() const;

    void setExpertsCubic(Surface value);
    Surface getExpertsCubic() const;

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

    void setDeviationAngle(Rad value);
    Rad getDeviationAngle() const;

    void setEntranceArmLength(double value);
    double getEntranceArmLength() const;

    void setExitArmLength(double value);
    double getExitArmLength() const;

    void setRadiusDirection(CylinderDirection value);
    CylinderDirection getRadiusDirection() const;

    // Setter and Getter for radius
    void setRadius(double value);
    double getRadius() const;

    void setCalcRadius();
    void setCalcRadiusDeviationAngle();

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

    void setLineDensity(double value);
    double getLineDensity() const;

    void setShortRadius(double value);
    double getShortRadius() const;

    // Setter and Getter for longRadius
    void setLongRadius(double value);
    double getLongRadius() const;

    void setFresnelZOffset(double value);
    double getFresnelZOffset() const;

    // Setter and Getter for designAlphaAngle
    void setDesignAlphaAngle(Rad value);
    Rad getDesignAlphaAngle() const;

    // Setter and Getter for designBetaAngle
    void setDesignBetaAngle(Rad value);
    Rad getDesignBetaAngle() const;

    // Setter and Getter for designOrderOfDiffraction
    void setDesignOrderOfDiffraction(double value);
    double getDesignOrderOfDiffraction() const;

    // Setter and Getter for designEnergy
    void setDesignEnergy(double value);
    double getDesignEnergy() const;

    // Setter and Getter for designSagittalEntranceArmLength
    void setDesignSagittalEntranceArmLength(double value);
    double getDesignSagittalEntranceArmLength() const;

    // Setter and Getter for designSagittalExitArmLength
    void setDesignSagittalExitArmLength(double value);
    double getDesignSagittalExitArmLength() const;

    // Setter and Getter for designMeridionalEntranceArmLength
    void setDesignMeridionalEntranceArmLength(double value);
    double getDesignMeridionalEntranceArmLength() const;

    // Setter and Getter for designMeridionalExitArmLength
    void setDesignMeridionalExitArmLength(double value);
    double getDesignMeridionalExitArmLength() const;

    // Setter and Getter for orderOfDiffraction
    void setOrderOfDiffraction(double value);
    double getOrderOfDiffraction() const;

    void setAdditionalOrder(double value);
    double getAdditionalOrder() const;

    void setImageType(double value);
    double getImageType() const;

    void setCurvatureType(CurvatureType value);
    CurvatureType getCurvatureType() const;

    void setBehaviourType(BehaviourType value);
    BehaviourType getBehaviourType() const;
};
}  // namespace RAYX