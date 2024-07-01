#pragma once

#include "Value.h"

namespace RAYX {

struct RAYX_API DesignSource {
    DesignMap m_elementParameters;
    std::vector<Ray> compile(int thread_count) const;

    void setStokeslin0(double value);
    void setStokeslin45(double value);
    void setStokescirc(double value);
    glm::dvec4 getStokes() const;

    void setName(std::string s);
    void setType(std::string s);
    std::string getName() const;
    std::string getType() const;

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

    void setWorldPosition(glm::dvec4 p);
    glm::dvec4 getWorldPosition() const;

    void setWorldOrientation(glm::dmat4x4 o);
    glm::dmat4x4 getWorldOrientation() const;

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
