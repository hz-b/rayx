#pragma once

#include "Design/SlopeError.h"
#include "Model/SlopeError.h"
#include "ToModelTrait.h"

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
