#pragma once
#include <Data/xml.h>
#include <Tracer/Vulkan/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Toroid.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const char* name, Geometry::GeometricalShape geometricalShape,
                 const double width, const double height,
                 const double azimuthalAngle, glm::dvec4 position,
                 glm::dmat4x4 orientation, const double incidenceAngle,
                 const double mEntrance, const double mExit,
                 const double sEntrance, const double sExit,
                 const std::array<double, 7> slopeError, Material mat);

    ToroidMirror();
    ~ToroidMirror();

    static std::shared_ptr<ToroidMirror> createFromXML(xml::Parser);

    void calcRadius(double incidenceAngle);
    double getRadius() const;
    double getSagittalEntranceArmLength() const;
    double getSagittalExitArmLength() const;
    double getMeridionalEntranceArmLength() const;
    double getMeridionalExitArmLength() const;

  private:
    double m_sagittalEntranceArmLength;
    double m_sagittalExitArmLength;
    double m_meridionalEntranceArmLength;
    double m_meridionalExitArmLength;
    double m_longRadius;
    double m_shortRadius;
};

}  // namespace RAYX