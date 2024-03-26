#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };
struct DesignElement;  

Element makeCylinder(const DesignElement& dele);

}  // namespace RAYX
