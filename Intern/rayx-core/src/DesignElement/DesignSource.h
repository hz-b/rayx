#pragma once

//#include "Beamline/Objects/Objects.h"
//#include "Beamline/LightSource.h"
#include "Value.h"

namespace RAYX {

struct DesignSource {
    Value v;
    std::vector<Ray> compile(int thread_count) const;

    void setStokeslin0(double value);
    void setStokeslin45(double value);
    void setStokescirc(double value);
    glm::dvec4 getStokes() const;

    void setName(std::string s);
    void setType(std::string s);
    std::string getName() const;

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

    void setElectronEnergyOriantation(ElectronEnergyOrientation value);
    ElectronEnergyOrientation getElectronEnergyOrientation() const;

    void setSeperateEnergies(int value);

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

    

};
}
