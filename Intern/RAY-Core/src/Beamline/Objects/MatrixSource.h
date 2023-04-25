#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API MatrixSource : public LightSource {
  public:
    MatrixSource() = default;
    MatrixSource(const DesignObject&);
    virtual ~MatrixSource() = default;

    virtual std::vector<Ray> getRays() const override;
  private:
  std::array<double, 6> m_misalignmentParams;
  SourcePulseType m_sourceDistributionType;  //TODO: wo muss der name angepasst werden?

};

}  // namespace RAYX
