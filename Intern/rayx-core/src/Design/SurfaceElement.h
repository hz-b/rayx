#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx::design {

struct SurfaceElement {
    Area area;
    Behavior frontBehavior;
    Behavior backBehavior;
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

}  // namespace rayx
