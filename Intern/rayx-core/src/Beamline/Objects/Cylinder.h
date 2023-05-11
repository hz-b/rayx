#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };
Element makeCylinder(const DesignObject& dobj);

}  // namespace RAYX
