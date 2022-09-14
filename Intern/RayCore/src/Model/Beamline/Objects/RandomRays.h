#pragma once
#include <list>

#include "Model/Beamline/LightSource.h"

namespace RAYX {

class RAYX_API RandomRays : public LightSource {
  public:
    RandomRays(int low, int high, int numberOfRays);

    RandomRays() = default;
    ~RandomRays();

    std::vector<Ray> getRays() const override;
    void compareRays(std::vector<Ray*> input, std::vector<double> output) const;

  private:
    int m_low;
    int m_high;
    int m_numberOfRays;
};

}  // namespace RAYX
