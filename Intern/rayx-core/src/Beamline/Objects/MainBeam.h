#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API MainBeam : public LightSource {
  public:
    MainBeam(const DesignObject&);
    virtual ~MainBeam() = default;

    std::vector<Ray> getRays(int thread_count = 1) const override;

  private:

    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;
};

}  // namespace RAYX

