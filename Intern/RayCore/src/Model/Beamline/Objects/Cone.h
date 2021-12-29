#pragma once
#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {
class RAYX_API Cone : public OpticalElement {
  public:
    Cone(const char* name, Geometry::GeometricalShape geometricalShape,
         const double width, const double height, const double azimuthalAngle,
         glm::dvec4 position, glm::dmat4x4 orientation,
         const double grazingIncidence, const double entranceArmLength,
         const double exitArmLength, const std::vector<double> slopeError);

    Cone::Cone(const char* name, Geometry::GeometricalShape geometricalShape,
               const double upstream_radius_r,
               const double downstream_radius_rho, const double width,
               const double height, const double azimuthalAngle,
               glm::dvec4 position, glm::dmat4x4 orientation,
               const double grazingIncidence, const double entranceArmLength,
               const double exitArmLength,
               const std::vector<double> slopeError);
    ~Cone();
    void calcConePar(const double ZL, const double ra, const double rb,
                     const double th, double* R, double* RHO);
    double getIncidenceAngle() const;
    double getExitArmLength() const;
    double getRadius() const;
    double getEntranceArmLength() const;

  private:
    double m_radius;

    double m_incidence;
    double m_entranceArmLength;
    double m_exitArmLength;

    double m_downstreamRadius_rho;
    double m_upstreamRadius_R;
    double m_cm;

    double m_a11;
    double m_a22;
    double m_a23;
    double m_a24;
};

}  // namespace RAYX