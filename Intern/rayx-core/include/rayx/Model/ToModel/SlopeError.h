#pragma once

#include "ToModelTrait.h"
#include "rayx/Design/SlopeError.h"
#include "rayx/Model/SlopeError.h"

namespace rayx::detail {

template <>
struct ToModel<SlopeError> {
    static model::SlopeError apply(const SlopeError& slopeError) {
        return model::SlopeError{
            .saggital   = toModel(slopeError.saggital()),
            .meridional = toModel(slopeError.meridional()),
        };
    }
};

}  // namespace rayx::detail
