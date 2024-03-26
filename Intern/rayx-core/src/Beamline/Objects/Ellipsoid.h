#pragma once
#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class FigureRotation { Yes, Plane, A11 };
struct DesignElement;
Element makeEllipsoid(const DesignElement& dele);

}  // namespace RAYX
