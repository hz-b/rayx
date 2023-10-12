#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API PointSource : public LightSource {
  public:
    PointSource(const DesignObject&);
    virtual ~PointSource() = default;

    std::vector<Ray> getRays() const override;

  private:
    // Geometric Params
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
    SourcePulseType m_sourceDistributionType;  // TODO unused.

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    double m_verDivergence;
    double m_sourceDepth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
