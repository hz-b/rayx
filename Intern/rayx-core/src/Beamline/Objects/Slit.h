#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

// TODO CentralBeamstop could also be stored as a cutout, might be good for code re-use
enum class CentralBeamstop { None, Rectangle, Elliptical };  ///< central beamstop shape
Element makeSlit(const DesignObject& dobj);

}  // namespace RAYX
