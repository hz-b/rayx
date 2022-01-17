#pragma once
#include <Data/xml.h>
#include <Tracer/Vulkan/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API SphereGrating : public OpticalElement {
  public:
    // new, shortened constructor
    SphereGrating(const char* name, int mount,
                  Geometry::GeometricalShape geometricalShape, double width,
                  double height, const double azimuthalAngle, double radius,
                  glm::dvec4 position, glm::dmat4x4 orientation,
                  double designEnergyMounting, double lineDensity,
                  double orderOfDiffraction, std::array<double, 6> vls,
                  std::array<double, 7> slopeError, Material mat);

    SphereGrating();
    ~SphereGrating();

    static std::shared_ptr<SphereGrating> createFromXML(
        rapidxml::xml_node<>*, const std::vector<xml::Group>& group_context);

    // TODO (Theresa): should ideally be removed as soon as radius calculation
    // is simplified in GeometricUSerParams.cpp
    void calcRadius();
    void calcAlpha(double deviation, double normalIncidence);
    void focus(double angle);

    double getRadius() const;
    double getExitArmLength() const;
    double getEntranceArmLength() const;
    double getDeviation() const;  // not always calculated
    int getGratingMount() const;
    double getDesignEnergyMounting() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    double getA() const;
    std::array<double, 6> getVls() const;

  private:
    double m_radius;
    double m_entranceArmLength;
    double m_exitArmLength;
    double m_deviation;  // not always calculated
    GRATING_MOUNT m_gratingMount;
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
