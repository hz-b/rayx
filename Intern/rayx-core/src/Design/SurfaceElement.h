#pragma once

#include <optional>

#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "Aperture.h"
#include "SlopeError.h"

namespace rayx {

struct SurfaceElement {
    Area area;
    Behavior behaviour;
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

using Element = SurfaceElement;

}  // namespace rayx
