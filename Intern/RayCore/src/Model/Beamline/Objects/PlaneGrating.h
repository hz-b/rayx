#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API PlaneGrating : public OpticalElement {
  public:
    PlaneGrating(const DesignObject&);

    inline int getElementType() const { return TY_GRATING; }

    double getFixFocusConstantCFF();
    double getDesignEnergyMounting() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    std::array<double, 6> getVls();
    std::array<double, 16> getElementParams() const;

  private:
    int m_additionalOrder;
    double m_designEnergyMounting;
    double m_lineDensity;
    double m_orderOfDiffraction;
    std::array<double, 6> m_vls;
};

}  // namespace RAYX
