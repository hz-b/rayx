#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

// CentralBeamstop is ordered to be compatible with Ray-UI.
enum class CentralBeamstop { None, Rectangle, Elliptical };

Element makeSlit(const DesignObject& dobj);

}  // namespace RAYX
