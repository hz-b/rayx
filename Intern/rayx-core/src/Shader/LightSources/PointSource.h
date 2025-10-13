#pragma once

#include "LightSource.h"
#include "Shader/Rand.h"

namespace RAYX {

class RAYX_API PointSource : public LightSourceBase {
  public:
    PointSource(const DesignSource&);

    RAYX_FN_ACC detail::Ray genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                   Rand& __restrict rand) const;

  private:
    // Geometric Params
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
    // SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_pol;

    double m_horDivergence;
    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
