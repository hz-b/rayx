#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

// CentralBeamstop is ordered to be compatible with Ray-UI.
enum class CentralBeamstop { None, Rectangle, Elliptical };
struct DesignElement;  

Element makeSlit(const DesignElement& dele);

}  // namespace RAYX
