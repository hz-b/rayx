#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class FigureRotation { Yes, Plane, A11 };
Element makeEllipsoid(const DesignObject& dobj);

}  // namespace RAYX
