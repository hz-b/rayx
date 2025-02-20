#pragma once

#include "Beamline/Node.h"
#include "Shader/Ray.h"
#include "Value.h"

namespace RAYX {

struct RAYX_API DesignSource : public BeamlineNode {
    DesignSource() = default;
    ~DesignSource() = default;
    // Delete copy constructor because shallow copies of DesignMap lead to unexpected behavior
    DesignSource(const DesignSource& other) = delete;
    DesignSource& operator=(const DesignSource& other) = delete;
    // Allow move
    DesignSource(DesignSource&& other) noexcept;
    DesignSource& operator=(DesignSource&& other) noexcept;
    // Allow intentional copies
    std::unique_ptr<BeamlineNode> clone() const override;

    DesignMap m_elementParameters;
    std::vector<Ray> compile(int numThreads, const glm::dvec4& groupPosition, const glm::dmat4& groupOrientation) const;
    bool isSource() const override { return true; }

    void setStokeslin0(double value);
    void setStokeslin45(double value);
    void setStokescirc(double value);
    glm::dvec4 getStokes() const;

    void setName(std::string s);
    void setType(ElementType s);
    std::string getName() const;
    ElementType getType() const;

    void setWidthDist(SourceDist value);
    SourceDist getWidthDist() const;

    void setHeightDist(SourceDist value);
    SourceDist getHeightDist() const;

    void setHorDist(SourceDist value);
    SourceDist getHorDist() const;

    void setVerDist(SourceDist value);
    SourceDist getVerDist() const;

    void setHorDivergence(double value);
    double getHorDivergence() const;

    void setVerDivergence(double value);
    double getVerDivergence() const;

    void setVerEBeamDivergence(double value);
    double getVerEBeamDivergence() const;

    void setSourceDepth(double value);
    double getSourceDepth() const;

    void setSourceHeight(double value);
    double getSourceHeight() const;

    void setSourceWidth(double value);
    double getSourceWidth() const;

    void setBendingRadius(double value);
    double getBendingRadius() const;

    void setEnergySpread(double value);
    double getEnergySpread() const;

    void setEnergySpreadType(SpreadType value);
    SpreadType getEnergySpreadType() const;

    void setEnergyDistributionType(EnergyDistributionType value);
    EnergyDistributionType getEnergyDistributionType() const;

    void setEnergyDistributionFile(std::string value);

    void setEnergySpreadUnit(EnergySpreadUnit value);
    EnergySpreadUnit getEnergySpreadUnit() const;

    void setElectronEnergy(double value);
    double getElectronEnergy() const;

    void setElectronEnergyOrientation(ElectronEnergyOrientation value);
    ElectronEnergyOrientation getElectronEnergyOrientation() const;

    void setSeparateEnergies(int value);

    void setEnergy(double value);
    double getEnergy() const;

    void setPhotonFlux(double value);
    double getPhotonFlux() const;

    EnergyDistribution getEnergyDistribution() const;

    void setMisalignment(Misalignment m);
    Misalignment getMisalignment() const;

    void setNumberOfRays(double value);
    double getNumberOfRays() const;

    void setPosition(glm::dvec4 p);
    glm::dvec4 getPosition() const override;

    void setOrientation(glm::dmat4x4 o);
    glm::dmat4x4 getOrientation() const override;

    void setNumOfCircles(int value);
    int getNumOfCircles() const;

    void setMaxOpeningAngle(Rad value);
    Rad getMaxOpeningAngle() const;

    void setMinOpeningAngle(Rad value);
    Rad getMinOpeningAngle() const;

    void setDeltaOpeningAngle(Rad value);
    Rad getDeltaOpeningAngle() const;

    void setSigmaType(SigmaType value);
    SigmaType getSigmaType() const;

    void setUndulatorLength(double value);
    double getUndulatorLength() const;

    void setElectronSigmaX(double value);
    double getElectronSigmaX() const;

    void setElectronSigmaXs(double value);
    double getElectronSigmaXs() const;

    void setElectronSigmaY(double value);
    double getElectronSigmaY() const;

    void setElectronSigmaYs(double value);
    double getElectronSigmaYs() const;
};
}  // namespace RAYX
