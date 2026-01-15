#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Core.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx::design {

struct SurfaceElement {
    Area area;
    Behavior behavior                  = DetectBehavior{};
    std::optional<Curvature> curvature = std::nullopt;
    // TODO: should a slits aperture work from behind? maybe we need frontAperture and backAperture here
    std::optional<Aperture> aperture     = std::nullopt;
    std::optional<SlopeError> slopeError = std::nullopt;
};

using Element    = std::variant<SurfaceElement>;
using ElementPtr = std::variant<std::shared_ptr<SurfaceElement>>;

}  // namespace rayx::design
