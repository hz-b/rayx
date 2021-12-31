#pragma once
#include <Data/xml.h>
#include <Tracer/Vulkan/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API SphereMirror : public OpticalElement {
  public:
    // calculate radius in this class
    SphereMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle,
                 const double grazingIncidenceAngle, glm::dvec4 position,
                 glm::dmat4x4 orientation, const double entranceArmLength,
                 const double exitArmLength,
                 const std::vector<double> slopeError, Material mat);
    // radius is precalculated and given as a parameter
    SphereMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle, double radius,
                 glm::dvec4 position, glm::dmat4x4 orientation,
                 const std::vector<double> slopeError, Material mat);
    SphereMirror();
    ~SphereMirror();

    static std::shared_ptr<SphereMirror> createFromXML(
        rapidxml::xml_node<>*, const std::vector<xml::Group>& group_context);

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