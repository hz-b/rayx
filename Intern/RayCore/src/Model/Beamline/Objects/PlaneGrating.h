#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API PlaneGrating : public OpticalElement {
  public:
    PlaneGrating(const char* name,
                 OpticalElement::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle, glm::dvec4 position,
                 glm::dmat4x4 orientation, const double designEnergyMounting,
                 const double lineDensity, const double orderOfDiffraction,
                 const int additionalZeroOrder, const std::array<double, 6> vls,
                 const std::array<double, 7> slopeError, Material mat);

    static std::shared_ptr<PlaneGrating> createFromXML(const xml::Parser&);

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
