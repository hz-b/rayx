#pragma once

#include "LightSource.h"
#include "Shader/Rand.h"

namespace RAYX {

class RAYX_API CircleSource : public ModelLightSource {
  public:
    CircleSource(const DesignSource&);

    RAYX_FN_ACC Ray genRay(const SourceId sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand) const;

    RAYX_FN_ACC glm::dvec3 getDirection(Rand& __restrict rand) const;

  private:
    // Geometric Params
    // SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_stokes;

    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

    int m_numOfCircles;
    Rad m_maxOpeningAngle;
    Rad m_minOpeningAngle;
    Rad m_deltaOpeningAngle;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
