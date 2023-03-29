#pragma once

#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API SphereMirror : public OpticalElement {
  public:
    SphereMirror(const DesignObject&);

    inline int getBehaviourType() const { return BTYPE_MIRROR; }

    void calcRadius();
    double getRadius() const;
    double getEntranceArmLength() const;
    double getExitArmLength() const;

  private:
    double m_radius;
    double m_entranceArmLength;
    double m_exitArmLength;
    // grazing incidence, in rad
    Rad m_grazingIncidenceAngle;
};

}  // namespace RAYX
