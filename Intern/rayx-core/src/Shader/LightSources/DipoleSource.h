#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct PsiAndStokes {
    glm::dvec4 stokes;
    double psi;
};

using Schwinger = std::array<double, 59>;

RAYX_ACC_CONSTEXPR
Schwinger schwingerLinearX = {1.e-4, 1.e-3, 2.e-3, 4.e-3, 6.e-3, 8.e-3, 1.e-2, 2.e-2, 3.e-2, 4.e-2, 5.e-2, 6.e-2, 7.e-2, 8.e-2, 9.e-2,
                              1.e-1, 0.15,  0.2,   0.25,  0.3,   0.35,  0.4,   0.45,  0.5,   0.55,  0.6,   0.65,  0.7,   0.75,  0.8,
                              0.85,  0.9,   1.0,   1.25,  1.5,   1.75,  2.0,   2.25,  2.5,   2.75,  3.0,   3.25,  3.5,   3.75,  4.0,
                              4.25,  4.5,   4.75,  5.0,   5.5,   6.0,   6.5,   7.0,   7.5,   8.0,   8.5,   9.0,   9.5,   10.0};

RAYX_ACC_CONSTEXPR
Schwinger schwingerLinearY = {973.0,    213.6,    133.6,    83.49,    63.29,    51.92,    44.5,     27.36,    20.45,    16.57,    14.03,    12.22,
                              10.85,    9.777,    8.905,    8.182,    5.832,    4.517,    3.663,    3.059,    2.607,    2.255,    1.973,    1.742,
                              1.549,    1.386,    1.246,    1.126,    1.02,     9.28e-1,  8.465e-1, 7.74e-1,  6.514e-1, 4.359e-1, 3.004e-1, 2.113e-1,
                              1.508e-1, 1.089e-1, 7.926e-2, 5.811e-2, 4.286e-2, 3.175e-2, 2.362e-2, 1.764e-2, 1.321e-2, 9.915e-3, 7.461e-3, 5.626e-3,
                              4.25e-3,  2.436e-3, 1.404e-3, 8.131e-4, 4.842e-4, 2.755e-4, 1.611e-4, 9.439e-5, 5.543e-5, 3.262e-5, 1.922e-5};

RAYX_ACC_CONSTEXPR
Schwinger schwingerLogX = {0.0001, 0.001, 0.002, 0.004, 0.006, 0.008, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
                           0.1,    0.15,  0.2,   0.25,  0.3,   0.35,  0.4,  0.45, 0.5,  0.55, 0.6,  0.65, 0.7,  0.75, 0.8,
                           0.85,   0.9,   1,     1.25,  1.5,   1.75,  2,    2.25, 2.5,  2.75, 3,    3.25, 3.5,  3.75, 4,
                           4.25,   4.5,   4.75,  5,     5.5,   6,     6.5,  7,    7.5,  8,    8.5,  9,    9.5,  10};

RAYX_ACC_CONSTEXPR
Schwinger schwingerLogY = {0.0973,   0.2136,     0.2672,    0.33396,    0.37974,   0.41536,     0.445,      0.5472,     0.6135,   0.6628,
                           0.7015,   0.7332,     0.7595,    0.78216,    0.80145,   0.8182,      0.8748,     0.9034,     0.91575,  0.9177,
                           0.91245,  0.902,      0.88785,   0.871,      0.85195,   0.8316,      0.8099,     0.7882,     0.765,    0.7424,
                           0.719525, 0.6966,     0.6514,    0.544875,   0.4506,    0.369775,    0.3016,     0.245025,   0.19815,  0.159803,
                           0.12858,  0.103188,   0.08267,   0.06615,    0.05284,   0.0421388,   0.0335745,  0.0267235,  0.02125,  0.013398,
                           0.008424, 0.00528515, 0.0033894, 0.00206625, 0.0012888, 0.000802315, 0.00049887, 0.00030989, 0.0001922};

struct DipoleSource : LightSource {
    DipoleSource() = default;
    DipoleSource(const DesignSource& designSource);

    RAYX_FN_ACC Ray getRay(int lightSourceId, Rand& rand) const;

    // calculate Ray-Information
    RAYX_FN_ACC glm::dvec3 getXYZPosition(double phi, Rand& rand) const;
    RAYX_FN_ACC double getEnergy(Rand& rand) const;
    RAYX_FN_ACC PsiAndStokes getPsiandStokes(double en, Rand& rand) const;

    // support functions
    RAYX_FN_ACC double schwinger(double energy) const;
    RAYX_FN_ACC double vDivergence(double energy, double sigv) const;
    RAYX_FN_ACC double getNormalFromRange(double range, Rand& rand) const;
    RAYX_FN_ACC static double bessel(double hnue, double zeta);

    RAYX_FN_ACC static double getInterpolation(double energy);
    RAYX_FN_ACC PsiAndStokes dipoleFold(double psi, double photonEnergy, double sigpsi, Rand& rand) const;
    RAYX_FN_ACC glm::dvec4 calcStokesSyn(double energy, double psi1, double psi2) const;

    // secondary support functions
    void setMaxIntensity(Rand& rand);
    void setMaxFlux();
    double calcSourcePathLength() const;
    double calcPhaseJitter() const;
    double calcHorDivDegrees() const;
    double calcHorDivSeconds() const;
    double calcFluxOrg() const;

    // Geometric Params
    double m_bendingRadius;
    ElectronEnergyOrientation m_electronEnergyOrientation;
    double m_photonFlux;

    double m_sourceHeight;
    double m_sourceWidth;

    glm::dvec4 m_stokes;
    double m_electronEnergy;
    double m_criticalEnergy;
    double m_photonEnergy;
    double m_verEbeamDivergence;
    double m_bandwidth;
    double m_gamma;
    double m_photonWaveLength;
    double m_sourcePulseLength;
    double m_energySpread;
    EnergySpreadUnit m_energySpreadUnit;
    double m_maxFlux;
    double m_maxIntensity;
};

}  // namespace RAYX
