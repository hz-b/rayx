#pragma once

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Common/Enums.h"
#include "Curvature.h"
#include "Design/Element.h"
#include "Model/Element.h"
#include "SlopeError.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<SurfaceElement> {
    static model::SurfaceElement apply(const SurfaceElement& element) {
        return model::SurfaceElement{
            .area             = toModel(element.area()),
            .behavior         = toModel(element.behavior()),
            .curvature        = toModel(element.curvature()),
            .aperture         = toModel(element.aperture()),
            .slopeError       = toModel(element.slopeError()),
            .backFaceBehavior = element.backFaceBehavior(),
        };
    }
};

}  // namespace rayx::detail
