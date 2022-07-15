#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API SphereMirror : public OpticalElement {
  public:
    // calculate radius in this class
    SphereMirror(const char* name, OpticalElement::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle,
                 const double grazingIncidenceAngle, glm::dvec4 position,
                 glm::dmat4x4 orientation, const double entranceArmLength,
                 const double exitArmLength,
                 const std::array<double, 7> slopeError, Material mat);
    // radius is precalculated and given as a parameter
    SphereMirror(const char* name, OpticalElement::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle, double radius,
                 glm::dvec4 position, glm::dmat4x4 orientation,
                 const std::array<double, 7> slopeError, Material mat);

    static std::shared_ptr<SphereMirror> createFromXML(xml::Parser);

    void calcRadius();
    double getRadius() const;
    double getEntranceArmLength() const;
    double getExitArmLength() const;

  private:
    double m_radius;
    double m_entranceArmLength;
    double m_exitArmLength;
    // grazing incidence, in rad
    double m_grazingIncidenceAngle;
};

}  // namespace RAYX