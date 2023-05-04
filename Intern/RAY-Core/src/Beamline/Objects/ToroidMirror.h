#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const DesignObject&);
};

}  // namespace RAYX
