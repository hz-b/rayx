#pragma once

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"
#include "rayx/Common/Enums.h"

namespace rayx::detail::model {

struct SurfaceElement {
    model::Area area;
    model::Behavior behavior;
    std::optional<model::Curvature> curvature;
    std::optional<model::Aperture> aperture;
    std::optional<model::SlopeError> slopeError;
    BackFaceBehavior backFaceBehavior;
};

}  // namespace rayx::detail::model
