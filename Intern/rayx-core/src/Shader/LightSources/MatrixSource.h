#pragma once

#include "LightSource.h"
#include "Shader/Rand.h"

namespace rayx {

class DesignSource;

class RAYX_API MatrixSource : public LightSourceBase {
  public:
    MatrixSource(const DesignSource&);

    RAYX_FN_ACC detail::Ray genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                   Rand& __restrict rand) const;

  private:
    glm::dvec4 m_pol;

    double m_horDivergence;
    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
};

}  // namespace rayx
