#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API SphereGrating : public OpticalElement {
  public:
    SphereGrating(const DesignObject&);
    inline int getElementType() const { return TYPE_GRATING; }

    void calcRadius();
    void calcAlpha(double deviation, double normalIncidence);
    void focus(double angle);

    double getRadius() const;
    double getExitArmLength() const;
    double getEntranceArmLength() const;
    double getDeviation() const;  // not always calculated
    GratingMount getGratingMount() const;
    double getDesignEnergyMounting() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    double getA() const;
    std::array<double, 6> getVls() const;
    std::array<double, 16> getElementParams() const;

  private:
    double m_radius;
    double m_entranceArmLength;
    double m_exitArmLength;
    double m_deviation;  // not always calculated
    GratingMount m_gratingMount;
    double m_designEnergyMounting;
    double m_lineDensity;
    double m_orderOfDiffraction;
    double m_a;  // calculated from line density, order of diffracion and design
                 // energy mounting
    std::array<double, 6> m_vls;
    // double m_Depth;
    // double m_verDivergence;
    // double m_horDivergence;
};

}  // namespace RAYX
