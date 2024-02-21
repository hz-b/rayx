#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

// CentralBeamstop is ordered to be compatible with Ray-UI.
enum class CentralBeamstop { None, Rectangle, Elliptical };
struct DesignElement; //TODO Fanny see where the forward declaration has to go

Element makeSlit(const DesignElement& dele);

}  // namespace RAYX
