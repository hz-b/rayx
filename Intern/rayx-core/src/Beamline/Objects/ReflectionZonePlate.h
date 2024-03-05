#pragma once

#include <optional>

#include "Beamline/OpticalElement.h"

namespace RAYX {
struct DesignElement;  

  // influences surface paramters for eg quadric function or (if torus)
    // newton iteration
enum class RZPType { Elliptical, Meriodional };

Element makeReflectionZonePlate(const DesignElement& dele);

}  // namespace RAYX
