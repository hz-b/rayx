#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API MatrixSource : public LightSource {
  public:
    MatrixSource(const DesignObject&);
    virtual ~MatrixSource() = default;

    virtual std::vector<Ray> getRays() const override;

  private:
    SourcePulseType m_sourceDistributionType;  // TODO: wo muss der name angepasst werden?

    float m_linearPol_0;
    float m_linearPol_45;
    float m_circularPol;

    float m_verDivergence;
    float m_sourceDepth;
};

}  // namespace RAYX
