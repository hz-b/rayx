#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {
struct DesignElement;  

Element makePlaneMirror(const DesignElement& dele);

}  // namespace RAYX
