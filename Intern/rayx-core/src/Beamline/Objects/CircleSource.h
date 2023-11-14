#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API CircleSource : public LightSource {
  public:
    CircleSource(const DesignObject&);
    virtual ~CircleSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;
    glm::dvec3 getDirection() const;

  private:
    // Geometric Params
    SourcePulseType m_sourceDistributionType;  // TODO unused.
    Misalignment m_misalignment;

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    double m_sourceDepth;

    int m_numOfCircles;
    Rad m_maxOpeningAngle;
    Rad m_minOpeningAngle;
    Rad m_deltaOpeningAngle;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
