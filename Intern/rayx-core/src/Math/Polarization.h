#pragma once

#include <glm/glm.hpp>

#include "detail/Adapt.h"
#include "Constants.h"

namespace rayx::math {

using Stokes = glm::dvec4;
using LocalElectricField = glm::tvec2<std::complex<double>>;
using ElectricField = glm::tvec3<std::complex<double>>;
static_assert(std::is_default_constructible_v<Stokes>);
static_assert(std::is_default_constructible_v<LocalElectricField>);
static_assert(std::is_default_constructible_v<ElectricField>);

/// specialize type trait `LocalElectricField_T` for std::complex
template <>
struct detail::LocalElectricField_T<std::complex> {
    using type = LocalElectricField;
};

/// specialize type trait `ElectricField_T` for std::complex
template <>
struct detail::ElectricField_T<std::complex> {
    using type = ElectricField;
};

template <template <typename> typename TComplex = std::complex>
RAYX_FN_ACC
inline detail::LocalElectricField_t<TComplex> toLocalElectricField(const Stokes stokes) {
    const auto x_real  = std::sqrt((stokes.x + stokes.y) / 2.0);
    const auto y_mag   = std::sqrt((stokes.x - stokes.y) / 2.0);
    const auto y_theta = -1.0 * std::atan2(stokes.w, stokes.z);
    const auto y       = detail::adapt<TComplex>::polar(y_mag, y_theta);
    return detail::LocalElectricField_t<TComplex>({x_real, 0}, y);
}

template <template <typename> typename TComplex = std::complex>
RAYX_FN_ACC
inline Stokes toStokes(const detail::LocalElectricField_t<TComplex> field) {
    const auto mag   = glm::dvec2(detail::adapt<TComplex>::abs(field.x), detail::adapt<TComplex>::abs(field.y));
    const auto theta = glm::dvec2(detail::adapt<TComplex>::arg(field.x), detail::adapt<TComplex>::arg(field.y));

    return Stokes(mag.x * mag.x + mag.y * mag.y, mag.x * mag.x - mag.y * mag.y, 2.0 * mag.x * mag.y * std::cos(theta.x - theta.y),
                  2.0 * mag.x * mag.y * std::sin(theta.x - theta.y));
}

}  // namespace rayx::math
