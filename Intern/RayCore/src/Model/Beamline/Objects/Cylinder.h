#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"
#include "utils.h"

namespace RAYX {
enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

class RAYX_API Cylinder : public OpticalElement {
  public:
    Cylinder(const char* name,
             OpticalElement::GeometricalShape geometricalShape,
             const double radius, CylinderDirection direction,
             const double width, const double height,
             const double azimuthalAngle, glm::dvec4 position,
             glm::dmat4x4 orientation, const double grazingIncidence,
             const double entranceArmLength, const double exitArmLength,
             const std::array<double, 7> slopeError, Material mat);
    ~Cylinder();

    static std::shared_ptr<Cylinder> createFromXML(const xml::Parser&);

    void setRadius();
    CylinderDirection getDirection() const;

    double getIncidenceAngle() const;
    double getExitArmLength() const;
    double getRadius() const;
    double getEntranceArmLength() const;

  private:
    // user params:
    CylinderDirection m_direction;  // Cylinder in z-/x- Direction
    double m_radius;                // Radius

    double m_incidence;
    double m_entranceArmLength;
    double m_exitArmLength;

    double m_a11;  // paramters for quadric equation
    double m_a33;
    double m_a24;
};
}  // namespace RAYX
