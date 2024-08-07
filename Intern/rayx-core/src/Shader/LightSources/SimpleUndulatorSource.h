#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct SimpleUndulatorSource : LightSource {
    SimpleUndulatorSource() = default;
    inline SimpleUndulatorSource(const DesignSource& designSource);

    RAYX_FN_ACC
    double getCoord(const double extent, Rand& rand) const;

    RAYX_FN_ACC
    Ray getRay(int32_t lightSourceId, Rand& rand) const;

    double calcUndulatorSigma() const;
    double calcUndulatorSigmaS() const;
    double getHorDivergence() const;
    double getSourceHeight() const;
    double getSourceWidth() const;
    double getVerDivergence() const;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_energyDistribution;

    ElectricField m_field = {{}, {}, {}};

    SigmaType m_sigmaType;
    double m_undulatorLength;
    double m_undulatorSigma;
    double m_undulatorSigmaS;
    double m_photonEnergy;
    double m_photonWaveLength;

    double m_electronSigmaX;
    double m_electronSigmaXs;
    double m_electronSigmaY;
    double m_electronSigmaYs;

    SourcePulseType m_sourceDistributionType;  // TODO unused.
};

SimpleUndulatorSource::SimpleUndulatorSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_energyDistribution(designSource.getEnergyDistribution()),
      m_field(stokesToElectricField(designSource.getStokes())),
      m_sigmaType(designSource.getSigmaType()),
      m_undulatorLength(designSource.getUndulatorLength()),
      m_photonEnergy(designSource.getEnergy()),
      m_photonWaveLength(calcPhotonWavelength(m_photonEnergy)),
      m_electronSigmaX(designSource.getElectronSigmaX()),
      m_electronSigmaXs(designSource.getElectronSigmaXs()),
      m_electronSigmaY(designSource.getElectronSigmaY()),
      m_electronSigmaYs(designSource.getElectronSigmaYs())

{
    m_undulatorSigma = calcUndulatorSigma();
    m_undulatorSigmaS = calcUndulatorSigmaS();
    m_horDivergence = getHorDivergence();
    m_verDivergence = getVerDivergence();
    m_sourceHeight = getSourceHeight();
    m_sourceWidth = getSourceWidth();
}

}  // namespace RAYX
