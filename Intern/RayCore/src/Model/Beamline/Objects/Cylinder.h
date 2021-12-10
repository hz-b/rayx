#pragma once
#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {
class RAYX_API Cylinder : public OpticalElement {
  public:
    Cylinder(const char* name, Geometry::GeometricalShape geometricalShape,
             const double radius, const int direction, const double width,
             const double height, glm::dvec4 position, glm::dmat4x4 orientation,
             const double grazingIncidence, const double entranceArmLength,
             const double exitArmLength, const std::vector<double> slopeError);
    ~Cylinder();

    double getRadius();
    enum CYLINDER_DIRECTION { X_DIR, Y_DIR };
    CYLINDER_DIRECTION getDirection();

  private:
    // user params:
    CYLINDER_DIRECTION m_direction;  // Cylinder in z-/x- Direction
    double m_radius;                 // Radius

    double m_incidence;
    double m_entranceArmLength;
    double m_exitArmLength;

    double m_a11;  // paramters for quadric equation
    double m_a33;
    double m_a24;
};
}  // namespace RAYX