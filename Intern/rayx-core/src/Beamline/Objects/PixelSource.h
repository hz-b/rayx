#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API PixelSource : public LightSource {
  public:
    PixelSource(const DesignSource&);
    virtual ~PixelSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;

  private:
    // Geometric Params
    SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_pol;

    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
