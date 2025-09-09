#pragma once

#include "LightSource.h"
#include "Shader/Rand.h"

namespace RAYX {

class RAYX_API SimpleUndulatorSource : public ModelLightSource {
  public:
    SimpleUndulatorSource(const DesignSource&);

    RAYX_FN_ACC Ray genRay(const SourceId sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand) const;

    RAYX_FN_ACC double getCoord(const double extent, Rand& __restrict rand) const;

  private:
    // Geometric Params
    double m_horDivergence;
    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

    // double m_undulatorSigma;
    // double m_undulatorSigmaS;
    double m_photonEnergy;
    double m_photonWaveLength;

    // SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_pol;

    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
