#pragma once
#include <list>

#include "Model/Beamline/LightSource.h"

namespace RAYX {

class RAYX_API RandomRays : public LightSource {
  public:
    RandomRays(int low, int high, int numberOfRays);
    ~RandomRays();

    std::vector<Ray> getRays() const override;

  private:
    int m_low;
    int m_high;
};

}  // namespace RAYX
