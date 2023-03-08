#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

class RAYX_API PlaneMirror : public OpticalElement {
  public:
    PlaneMirror(const DesignObject&);

    inline int getElementType() const { return TY_MIRROR; }
};

}  // namespace RAYX
