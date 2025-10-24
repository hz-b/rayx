#pragma once

#include "LightSource.h"
#include "Shader/Rand.h"

namespace rayx {

class RAYX_API PixelSource : public LightSourceBase {
  public:
    PixelSource(const DesignSource&);

    RAYX_FN_ACC detail::Ray genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                   Rand& __restrict rand) const;

  private:
    // Geometric Params
    // SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_pol;

    double m_horDivergence;
    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace rayx
