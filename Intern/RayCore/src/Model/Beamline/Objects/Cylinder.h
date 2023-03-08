#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {
enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

class RAYX_API Cylinder : public OpticalElement {
  public:
    Cylinder(const DesignObject&);
    ~Cylinder() = default;

    inline int getElementType() const { return TY_MIRROR; }

    void setRadius();
    CylinderDirection getDirection() const;

    Rad getIncidenceAngle() const;
    double getExitArmLength() const;
    double getRadius() const;
    double getEntranceArmLength() const;

  private:
    // user params:
    CylinderDirection m_direction;  // Cylinder in z-/x- Direction
    double m_radius;                // Radius

    Rad m_incidence;
    double m_entranceArmLength;
    double m_exitArmLength;

    double m_a11;  // paramters for quadric equation
    double m_a33;
    double m_a24;
};
}  // namespace RAYX
