#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx {

struct SurfaceElement {
    Area area;
    Behavior behavior;
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

using Element = SurfaceElement;

}  // namespace rayx
