#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

struct DesignElement;
enum class ParaboloidDirection { LongRadiusR, ShortRadiusRho };
Element makeParaboloid(const DesignElement& dele);

}  // namespace RAYX
