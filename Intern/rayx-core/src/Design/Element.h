#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Core.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx {

struct SurfaceElement {
    SurfaceElement(Area area, Behavior behavior) : m_area(area), m_behavior(behavior) {}

    RAYX_NESTED_PROPERTY(SurfaceElement, Area, area);
    RAYX_NESTED_PROPERTY(SurfaceElement, Behavior, behavior);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<Curvature>, curvature);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<Aperture>, aperture);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<SlopeError>, slopeError);
    RAYX_PROPERTY(SurfaceElement, BackFaceBehavior, backFaceBehavior) = BackFaceBehavior::Absorb;
};

using Element    = std::variant<SurfaceElement>;
using ElementPtr = std::variant<std::shared_ptr<SurfaceElement>>;

}  // namespace rayx
