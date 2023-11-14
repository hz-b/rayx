#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class ParaboloidDirection { LongRadiusR, ShortRadiusRho };
Element makeParaboloid(const DesignObject& dobj);

}  // namespace RAYX
