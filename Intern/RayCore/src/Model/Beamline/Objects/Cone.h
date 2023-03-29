#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {
class RAYX_API Cone : public OpticalElement {
  public:
    Cone(const DesignObject&);

    inline int getBehaviourType() const { return BTYPE_MIRROR; }

    ~Cone();
    void calcConeParams(double zl);
    Rad getIncidenceAngle() const;
    double getEntranceArmLength() const;
    double getExitArmLength() const;
    double getR() const;
    double getRHO() const;

  private:
    Rad m_incidence;
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
