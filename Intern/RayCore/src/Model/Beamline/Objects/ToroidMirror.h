#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Toroid.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const DesignObject&);
    double getRadius() const;

  private:
    double m_longRadius = 1000;
    double m_shortRadius = 1000;
};

}  // namespace RAYX
