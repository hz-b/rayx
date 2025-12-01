#pragma once

#include <optional>

#include "Object.h"
#include "Area.h"
#include "Behaviour.h"
#include "Curvature.h"
#include "Aperture.h"
#include "SlopeError.h"

namespace rayx {

struct SurfaceElement : Object {
    Area area;
    Behaviour behaviour;
    std::optional<Curvature> curvature;
    std::optional<Aperture> aperture;
    std::optional<SlopeError> slopeError;
};

}  // namespace rayx
