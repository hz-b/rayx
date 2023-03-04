#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const DesignObject&);

    inline int getElementType() const { return TY_TOROID_MIRROR; }
    double getRadius() const;

  private:
    double m_longRadius = 1000;
    double m_shortRadius = 1000;
};

}  // namespace RAYX
