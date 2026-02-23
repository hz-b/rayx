#pragma once

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "Design/Element.h"
#include "SlopeError.h"
#include "ToModel.h"

namespace rayx::detail::model {

struct SurfaceElement {
    model::Area area;
    model::Behavior behavior;
    std::optional<model::Curvature> curvature;
    std::optional<model::Aperture> aperture;
    std::optional<model::SlopeError> slopeError;
};

}  // namespace rayx::detail::model

namespace rayx::detail {

model::SurfaceElement toModel(const SurfaceElement& element);

}
