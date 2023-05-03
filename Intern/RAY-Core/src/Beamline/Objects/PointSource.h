#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API PointSource : public LightSource {
  public:
    PointSource() = default;
    PointSource(const DesignObject&);
    virtual ~PointSource() = default;

    std::vector<Ray> getRays() const override;

  private:
    // Geometric Params
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
    SourcePulseType m_sourceDistributionType;
    std::array<double, 6> m_misalignmentParams;

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
