#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API MatrixSource : public LightSource {
  public:
    MatrixSource(const DesignObject&);
    virtual ~MatrixSource() = default;

    virtual std::vector<Ray> getRays(int thread_count = 1) const override;

  private:
    SourcePulseType m_sourceDistributionType;  // TODO: wo muss der name angepasst werden?

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    double m_verDivergence;
    double m_sourceDepth;
};

}  // namespace RAYX
