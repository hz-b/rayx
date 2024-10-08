#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {
struct DesignSource;

class RAYX_API MatrixSource : public LightSource {
  public:
    MatrixSource(const DesignSource&);
    virtual ~MatrixSource() = default;

    virtual std::vector<Ray> getRays(int thread_count = 1) const override;

  private:
    // SourcePulseType m_sourceDistributionType;  // TODO: wo muss der name angepasst werden?

    glm::dvec4 m_pol;

    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
};

}  // namespace RAYX
