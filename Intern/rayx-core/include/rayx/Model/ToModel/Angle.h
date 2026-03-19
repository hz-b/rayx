#pragma once

#include "ToModelTrait.h"
#include "rayx/Design/Angle.h"

namespace rayx::detail {

template <>
struct ToModel<Deg> {
    static double apply(const Deg& deg) { return toRad(deg).value(); }
};

template <>
struct ToModel<Rad> {
    static double apply(const Rad& rad) { return rad.value(); }
};

template <>
struct ToModel<Angle> {
    static double apply(const Angle& angle) {
        return std::visit([](const auto arg) { return toModel(arg); }, angle);
    }
};

}  // namespace rayx::detail
