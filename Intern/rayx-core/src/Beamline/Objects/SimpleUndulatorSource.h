#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API SimpleUndulatorSource : public LightSource {
  public:
    SimpleUndulatorSource(const DesignObject&);
    virtual ~SimpleUndulatorSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;

    double calcUndulatorSigma() const;
    double calcUndulatorSigmaS() const;
    double getSourceHeight() const override;
    double getSourceWidth() const override;

    double getCoord(const SourceDist l, const double extent) const;

  private:
    // Geometric Params
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

    SigmaType m_sigmaType;
    double m_undulatorLength;
    double m_undulatorSigma;
    double m_photonEnergy;
    double m_photonWaveLength;

    double m_electronSigmaX;
    double m_electronSigmaXs;
    double m_electronSigmaY;
    double m_electronSigmaYs;

    SourcePulseType m_sourceDistributionType;  // TODO unused.

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
