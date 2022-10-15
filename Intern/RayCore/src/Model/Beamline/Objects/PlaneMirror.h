#pragma once
#include <Data/xml.h>
#include <Material/Material.h>

#include <vector>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API PlaneMirror : public OpticalElement {
  public:
    PlaneMirror(const DesignObject&);
};

}  // namespace RAYX
