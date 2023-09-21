#pragma once

#include <list>

#include "Beamline/LightSource.h"

namespace RAYX {

struct PsiAndStokes{
    glm::vec4 stokes;
    float psi; 
};


class RAYX_API DipoleSource : public LightSource {
  public:
    DipoleSource(const DesignObject&);
    virtual ~DipoleSource() = default;

    std::vector<Ray> getRays() const override;

    void calcMagneticField();
    void calcWorldCoordinates();
    void calcSourcePath();
    void calcHorDivDegSec();  // horizontal Divergence as degree and seconds
    void calcPhotonWavelength();
    void calcFluxOrg();
    float schwinger(float) const;
    void setMaxFlux();
    void setLogInterpolation();
    float getInterpolation(float) const;
    float getEnergy() const;
    PsiAndStokes getPsiandStokes(float) const;
    void setMaxIntensity();
    glm::vec3 getXYZPosition(float)const;
    float vDivergence(float hv, float sigv) const;
    float getNormalFromRange(float range) const;

  private:
    // Geometric Params
    float m_bendingRadius;
    ElectronEnergyOrientation m_electronEnergyOrientation;
    SourcePulseType m_sourcePulseType;
    float m_photonFlux;
    EnergyDistribution m_energySpreadType;

    float m_sigpsi;
    glm::vec4 m_stokes;
    float m_electronEnergy;
    float m_criticalEnergy;
    float m_photonEnergy;
    float m_verEbeamDivergence;
    float m_flux;
    float m_totalPower;
    float m_bandwidth;  // default bandwidth
    float m_magneticFieldStrength;
    float m_gamma;
    float m_beta;
    float m_photonWaveLength;
    float m_sourcePulseLength;
    float m_sourcePathLength;
    float m_phaseJitter;
    float m_horDivDegrees;
    float m_horDivSeconds;
    float m_energySpread;
    EnergySpreadUnit m_energySpreadUnit;
    float m_photonFluxOrg;
    float m_maxFlux;
    float m_maxIntensity;

    
    glm::vec4 getStokesSyn(float hv, float psi1, float psi2) const;
    float bessel(float hnue, float zeta) const;
    glm::vec4 dipoleFold(float psi, float hv, float sigpsi) const;

    std::array<float, 59> m_schwingerX = {1.e-4, 1.e-3, 2.e-3, 4.e-3, 6.e-3, 8.e-3, 1.e-2, 2.e-2, 3.e-2, 4.e-2, 5.e-2, 6.e-2, 7.e-2, 8.e-2, 9.e-2,
                                           1.e-1, 0.15,  0.2,   0.25,  0.3,   0.35,  0.4,   0.45,  0.5,   0.55,  0.6,   0.65,  0.7,   0.75,  0.8,
                                           0.85,  0.9,   1.0,   1.25,  1.5,   1.75,  2.0,   2.25,  2.5,   2.75,  3.0,   3.25,  3.5,   3.75,  4.0,
                                           4.25,  4.5,   4.75,  5.0,   5.5,   6.0,   6.5,   7.0,   7.5,   8.0,   8.5,   9.0,   9.5,   10.0};

    std::array<float, 59> m_schwingerY = {973.0,    213.6,    133.6,    83.49,    63.29,    51.92,    44.5,     27.36,    20.45,    16.57,
                                           14.03,    12.22,    10.85,    9.777,    8.905,    8.182,    5.832,    4.517,    3.663,    3.059,
                                           2.607,    2.255,    1.973,    1.742,    1.549,    1.386,    1.246,    1.126,    1.02,     9.28e-1,
                                           8.465e-1, 7.74e-1,  6.514e-1, 4.359e-1, 3.004e-1, 2.113e-1, 1.508e-1, 1.089e-1, 7.926e-2, 5.811e-2,
                                           4.286e-2, 3.175e-2, 2.362e-2, 1.764e-2, 1.321e-2, 9.915e-3, 7.461e-3, 5.626e-3, 4.25e-3,  2.436e-3,
                                           1.404e-3, 8.131e-4, 4.842e-4, 2.755e-4, 1.611e-4, 9.439e-5, 5.543e-5, 3.262e-5, 1.922e-5};

};
}  // namespace RAYX
