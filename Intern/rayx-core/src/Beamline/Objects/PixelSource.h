#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API PixelSource : public LightSource {
  public:
    PixelSource(const DesignObject&);
    virtual ~PixelSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;
    double getHorDivergence() const override;
    double getSourceHeight() const override;
    double getSourceWidth() const override;

  private:
    // Geometric Params
    SourcePulseType m_sourceDistributionType;  // TODO unused.

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
