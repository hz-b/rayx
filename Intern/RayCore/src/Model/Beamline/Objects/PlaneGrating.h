#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API PlaneGrating : public OpticalElement {
  public:
    PlaneGrating(const DesignObject&);

    double getFixFocusConstantCFF();
    double getDesignEnergyMounting() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    std::array<double, 6> getVls();
    glm::dmat4x4 getElementParameters() const;

  private:
    int m_additionalOrder;
    double m_designEnergyMounting;
    double m_lineDensity;
    double m_orderOfDiffraction;
    std::array<double, 6> m_vls;
};

}  // namespace RAYX
