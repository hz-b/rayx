#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API SphereMirror : public OpticalElement {
  public:
    SphereMirror(const DesignObject&);

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
