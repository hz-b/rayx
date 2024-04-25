#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {
struct DesignElement;  
Element makeToroidMirror(const DesignElement& dele);

}  // namespace RAYX
