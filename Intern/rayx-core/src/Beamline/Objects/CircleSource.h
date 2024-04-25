#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API CircleSource : public LightSource {
  public:
    CircleSource(const DesignSource&);
    virtual ~CircleSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;


    glm::dvec3 getDirection() const;

  private:
    // Geometric Params
    SourcePulseType m_sourceDistributionType;  // TODO unused.
    Misalignment m_misalignment;

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
