#pragma once
#include "Beamline/OpticalElement.h"
#include "DesignElement/DesignElement.h"

namespace RAYX {
struct DesignElement;  
Element makeImagePlane(const DesignElement& dele);

}  // namespace RAYX
