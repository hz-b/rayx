#pragma once

#include "Design/SlopeError.h"
#include "ToModel.h"

namespace rayx::detail::model {

struct SlopeError {
    double saggital   = 0.0;
    double meridional = 0.0;
};

}  // namespace rayx::detail::model

namespace rayx::detail {
inline model::SlopeError toModel(const SlopeError& slopeError) {
    return model::SlopeError{
        .saggital   = slopeError.saggital(),
        .meridional = slopeError.meridional(),
    };
}
}  // namespace rayx::detail
