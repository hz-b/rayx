#pragma once

#include <list>

#include "LightSource.h"
#include "Shader/Rand.h"

namespace RAYX {

struct PsiAndStokes {
    glm::dvec4 stokes;
    double psi;
};

RAYX_API double get_factorCriticalEnergy();
RAYX_API double get_factorElectronEnergy();
RAYX_API double get_factorOmega();
RAYX_API RAYX_FN_ACC double dipoleBessel(double hnue, double zeta);
RAYX_API RAYX_FN_ACC glm::dvec4 getStokesSyn(double energy, double psi1, double psi2, double electronEnergy, double criticalEnergy,
                                             ElectronEnergyOrientation electronEnergyOrientation);
RAYX_API RAYX_FN_ACC PsiAndStokes calcDipoleFold(double psi, double photonEnergy, double sigpsi, double electronEnergy, double criticalEnergy,
                                                 ElectronEnergyOrientation electronEnergyOrientation, Rand& __restrict rand);
RAYX_API double calcMaxIntensity(double photonEnergy, double verDivergence, double electronEnergy, double criticalEnergy,
                                 ElectronEnergyOrientation electronEnergyOrientation, Rand& __restrict rand);
RAYX_API double calcVerDivergence(double energy, double sigv, double electronEnergy, double criticalEnergy);
RAYX_API RAYX_FN_ACC double getDipoleInterpolation(double energy);
/// calculate probability for chosen energy with edge-cases according to H.Wiedemann Synchrotron Radiation P. 259 (D.21)
RAYX_API RAYX_FN_ACC double schwinger(double energy, double gamma, double criticalEnergy);
RAYX_API double calcMaxFlux(double photonEnergy, double energySpread, double criticalEnergy, double gamma);
RAYX_API double calcGamma(double electronEnergy);

class RAYX_API DipoleSource : public LightSourceBase {
  public:
    DipoleSource(const DesignSource&);

    RAYX_FN_ACC Ray genRay(const SourceId sourceId, Rand& __restrict rand) const;

  private:
    // calculate Ray-Information
    RAYX_FN_ACC glm::dvec3 getXYZPosition(double, Rand& __restrict rand) const;
    RAYX_FN_ACC PsiAndStokes getPsiandStokes(double, Rand& __restrict rand) const;

    // support functions
    RAYX_FN_ACC double getNormalFromRange(double range, Rand& __restrict rand) const;
    RAYX_FN_ACC double getEnergy(Rand& __restrict rand) const;

    // Geometric Params
    double m_bendingRadius;
    ElectronEnergyOrientation m_electronEnergyOrientation;
    // SourcePulseType m_sourcePulseType;
    // double m_photonFlux;
    // EnergyDistribution m_energySpreadType;

    double m_sourceHeight;
    double m_sourceWidth;

    // double m_sigpsi;
    // glm::dvec4 m_stokes;
    double m_electronEnergy;
    double m_criticalEnergy;
    double m_photonEnergy;
    double m_verEbeamDivergence;
    double m_flux;
    // double m_totalPower;
    // double m_bandwidth;
    // double m_magneticFieldStrength;
    double m_gamma;
    // double m_beta;
    double m_photonWaveLength;
    // double m_sourcePulseLength;
    // double m_sourcePathLength;
    // double m_phaseJitter;
    // double m_horDivDegrees;
    // double m_horDivSeconds;
    double m_energySpread;
    // EnergySpreadUnit m_energySpreadUnit;
    // double m_photonFluxOrg;
    double m_maxFlux;
    double m_maxIntensity;
    double m_horDivergence;
    double m_verDivergence;
};

}  // namespace RAYX
