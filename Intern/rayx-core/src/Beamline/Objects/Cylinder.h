#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };
struct DesignElement; //TODO Fanny see where the forward declaration has to go

Element makeCylinder(const DesignElement& dele);

}  // namespace RAYX
