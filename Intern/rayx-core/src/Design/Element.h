#pragma once

#include <optional>

#include "Core.h"
#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx::design {

struct SurfaceElement {
    std::optional<std::string> name;
    Area area;
    Behavior frontBehavior = DetectBehavior{};
    Behavior backBehavior = AbsorbBehavior{}; // TODO: consider IgnoreBehavior as default
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

using Element = std::variant<SurfaceElement>;
using ElementPtr = std::variant<std::shared_ptr<SurfaceElement>>;

}  // namespace rayx::design
