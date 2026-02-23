#include "Element.h"

namespace rayx::detail {

model::SurfaceElement toModel(const SurfaceElement& element) {
    return model::SurfaceElement{
        .area       = toModel(element.area()),
        .behavior   = toModel(element.behavior()),
        .curvature  = element.curvature() ? std::make_optional(toModel(*element.curvature())) : std::nullopt,
        .aperture   = element.aperture() ? std::make_optional(toModel(*element.aperture())) : std::nullopt,
        .slopeError = element.slopeError() ? std::make_optional(toModel(*element.slopeError())) : std::nullopt,
    };
}

}  // namespace rayx::detail
