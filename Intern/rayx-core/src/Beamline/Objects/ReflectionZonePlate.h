#pragma once

#include <optional>

#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class CurvatureType {
    Plane,
    Toroidal,
    Spherical
};  // influences surface paramters for eg quadric function or (if torus)
    // newton iteration
enum class DesignType { ZOffset, Beta };                  // TODO(Jannis): remove (default is Beta)
enum class ElementOffsetType { Manual, Beamdivergence };  // TODO(Jannis): remove (included in world coordinates)
enum class RZPType { Elliptical, Meriodional };

Element makeReflectionZonePlate(const DesignObject& dobj);

}  // namespace RAYX
