#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API ToroidMirror : public OpticalElement {
  public:
    ToroidMirror(const DesignObject&);

    inline int getElementType() const { return TYPE_MIRROR; }
};

}  // namespace RAYX
