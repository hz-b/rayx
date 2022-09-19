#pragma once
#include <Data/xml.h>
#include <Material/Material.h>
#include <UserParameter/GeometricUserParams.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API SphereGrating : public OpticalElement {
  public:
    // new, shortened constructor
    SphereGrating(const char* name, GratingMount mount,
                  OpticalElement::GeometricalShape geometricalShape, double width,
                  double height, const double azimuthalAngle, double radius,
                  glm::dvec4 position, glm::dmat4x4 orientation,
                  double designEnergyMounting, double lineDensity,
                  double orderOfDiffraction, std::array<double, 6> vls,
                  std::array<double, 7> slopeError, Material mat);

    static std::shared_ptr<SphereGrating> createFromXML(const xml::Parser&);

    // TODO (Theresa): should ideally be removed as soon as radius calculation
    // is simplified in GeometricUSerParams.cpp
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
    std::array<double, 4*4> getElementParameters() const;

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
