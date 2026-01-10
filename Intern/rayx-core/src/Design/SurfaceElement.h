#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx::design {

struct SurfaceElement {
    std::optional<std::string> name;
    Area area;
    Behavior behavior;
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

using ElementPtr = std::variant<std::shared_ptr<SurfaceElement>>;

}  // namespace rayx::design
