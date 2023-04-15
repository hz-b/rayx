#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API PlaneGrating : public OpticalElement {
  public:
    PlaneGrating(const DesignObject&);

    double getFixFocusConstantCFF();
    double getDesignEnergyMounting() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    std::array<double, 6> getVls();

  private:
    int m_additionalOrder;
    double m_designEnergyMounting;
    double m_lineDensity;
    double m_orderOfDiffraction;
    std::array<double, 6> m_vls;
};

}  // namespace RAYX
