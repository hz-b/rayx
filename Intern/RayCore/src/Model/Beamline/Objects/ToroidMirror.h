#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Toroid.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle, glm::dvec4 position,
                 glm::dmat4x4 orientation,
                 const double longRadius, const double shortRadius,           
                 const std::array<double, 7> slopeError, Material mat);

    ToroidMirror();
    ~ToroidMirror();

    static std::shared_ptr<ToroidMirror> createFromXML(xml::Parser);

    double getRadius() const;

  private:
    double m_longRadius = 1000;
    double m_shortRadius = 1000;
};

}  // namespace RAYX