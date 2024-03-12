#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API SimpleUndulatorSource : public LightSource {
  public:
    SimpleUndulatorSource(const DesignSource&);
    virtual ~SimpleUndulatorSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;

    double calcUndulatorSigma() const;
    double calcUndulatorSigmaS() const;

    double getHorDivergence() const;
    double getSourceHeight() const;
    double getSourceWidth() const;

    double getVerDivergence() const;

    double getCoord( double extent) const;

  private:
    // Geometric Params
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

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

    glm::dvec4 m_pol;

    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
