#pragma once

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Curvature.h"
#include "SlopeError.h"
#include "ToModelTrait.h"
#include "rayx/Common/Enums.h"
#include "rayx/Design/Element.h"
#include "rayx/Model/Element.h"

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
