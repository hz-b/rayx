#pragma once

#include "Beamline/LightSource.h"

#include<list> 

namespace RAYX {

class RAYX_API DipoleSource : public LightSource {
  public:
    DipoleSource() = default;
    DipoleSource(const DesignObject&);
    virtual ~DipoleSource() = default;

    std::vector<Ray> getRays() const override;


  private:
    // Geometric Params
    double m_bendingRadius;
    ElectronEnergyOrientation m_electronEnergyOrientation;
    SourcePulseType m_sourcePulseType;
    double m_photonFlux;
    EnergyDistribution m_energySpreadType;
    double d_sourceWidth;

    double d_sigpsi;
    std::array<double, 6> d_synStokes;
    double m_electronEnergy;
    double d_criticalEnergy;
    double m_photonEnergy;
    double m_verEbeamDivergence;
    double d_flux;
    double d_totalPower;
    double d_bandwidth;         //default bandwidth
    double d_magneticFieldStrength;
    double m_gamma;
    double m_beta;

    double vDivergence(double eel, double ec, double hv, double sigv);
    std::array<double, 6> syn(double eel, double ec, double hv, double psi1, double psi2);
    double bessel(double hnue,double zeta);
    void getMaxIntensity();
    std::array<double, 6> dipoleFold(double psi, double eel, double ec, double hv, double sigpsi);
    
    void calcMagneticField();
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
