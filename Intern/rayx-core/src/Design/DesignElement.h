#pragma once

#include "Beamline/Node.h"
#include "Element/Element.h"
#include "Value.h"

namespace RAYX {

class RAYX_API DesignElement : public BeamlineNode {
  public:
    DesignElement();
    DesignElement(std::string name);
    ~DesignElement() = default;

    // Delete copy constructor because shallow copies of DesignMap lead to unexpected behavior
    DesignElement(const DesignElement& other) = delete;
    DesignElement& operator=(const DesignElement& other) = delete;

    // Allow move
    DesignElement(DesignElement&& other) noexcept;
    DesignElement& operator=(DesignElement&& other) noexcept;
    // Allow intentional copies
    std::unique_ptr<BeamlineNode> clone() const override;

    DesignMap m_elementParameters;
    OpticalElementAndTransform compile(const glm::dvec4& groupPosition, const glm::dmat4& groupOrientation) const;

    bool isElement() const override { return true; }

    std::string getName() const override;
    void setName(std::string s) override;

    ElementType getType() const;
    void setType(ElementType s);

    void setPosition(glm::dvec4 p);
    glm::dvec4 getPosition() const override;

    void setOrientation(glm::dmat4x4 o);
    glm::dmat4x4 getOrientation() const override;

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

    void setOpeningShape(CutoutType shape);
    CutoutType getOpeningShape() const;

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

    void setDesignAlphaAngle(Rad value);
    Rad getDesignAlphaAngle() const;

    void setDesignBetaAngle(Rad value);
    Rad getDesignBetaAngle() const;

    void setDesignOrderOfDiffraction(int value);
    int getDesignOrderOfDiffraction() const;

    void setDesignEnergy(double value);
    double getDesignEnergy() const;

    void setDesignSagittalEntranceArmLength(double value);
    double getDesignSagittalEntranceArmLength() const;

    void setDesignSagittalExitArmLength(double value);
    double getDesignSagittalExitArmLength() const;

    // Setter and Getter for designMeridionalEntranceArmLength
    void setDesignMeridionalEntranceArmLength(double value);
    double getDesignMeridionalEntranceArmLength() const;

    // Setter and Getter for designMeridionalExitArmLength
    void setDesignMeridionalExitArmLength(double value);
    double getDesignMeridionalExitArmLength() const;

    // Setter and Getter for orderOfDiffraction
    void setOrderOfDiffraction(int value);
    int getOrderOfDiffraction() const;

    void setAdditionalOrder(int value);
    int getAdditionalOrder() const;

    void setImageType(int value);
    int getImageType() const;

    void setCurvatureType(CurvatureType value);
    CurvatureType getCurvatureType() const;

    void setBehaviourType(BehaviourType value);
    BehaviourType getBehaviourType() const;

    void setCrystalType(CrystalType value);
    CrystalType getCrystalType() const;

    void setCrystalMaterial(const std::string& value);
    std::string getCrystalMaterial() const;

    void setOffsetAngleType(OffsetAngleType value);
    OffsetAngleType getOffsetAngleType() const;

    void setOffsetAngle(Rad value);
    Rad getOffsetAngle() const;

    void setStructureFactorReF0(double value);
    double getStructureFactorReF0() const;

    void setStructureFactorImF0(double value);
    double getStructureFactorImF0() const;

    void setStructureFactorReFH(double value);
    double getStructureFactorReFH() const;

    void setStructureFactorImFH(double value);
    double getStructureFactorImFH() const;

    void setStructureFactorReFHC(double value);
    double getStructureFactorReFHC() const;

    void setStructureFactorImFHC(double value);
    double getStructureFactorImFHC() const;

    void setUnitCellVolume(double value);
    double getUnitCellVolume() const;

    void setDSpacing2(double value);
    double getDSpacing2() const;

    void setThicknessSubstrate(double value);
    double getThicknessSubstrate() const;

    void setRoughnessSubstrate(double value);
    double getRoughnessSubstrate() const;

    void setDesignPlane(DesignPlane value);
    DesignPlane getDesignPlane() const;

    void setSurfaceCoatingType(SurfaceCoatingType value);
    SurfaceCoatingType getSurfaceCoatingType() const;

    void setMultilayerCoating(const Coating::MultilayerCoating& coating);

    Coating getCoating() const;

    void setMaterialCoating(Material value);
    Material getMaterialCoating() const;

    void setThicknessCoating(double value);
    double getThicknessCoating() const;

    void setRoughnessCoating(double value);
    double getRoughnessCoating() const;
};
}  // namespace RAYX
