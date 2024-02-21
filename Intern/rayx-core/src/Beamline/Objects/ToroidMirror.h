#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {
struct DesignElement; //TODO Fanny see where the forward declaration has to go
Element makeToroidMirror(const DesignElement& dele);

}  // namespace RAYX
