#pragma once

#include <variant>

#include "Math/Polarization.h"

namespace rayx::design {

using Polarization = std::variant<math::Stokes, math::LocalElectricField>;

// conversion functions

inline math::Stokes toStokes(const math::LocalElectricField& localElectricField) {
    return math::toStokes(localElectricField);
}

inline math::Stokes toStokes(const math::Stokes& stokes) {
    return stokes;
}

inline math::Stokes toStokes(const Polarization& polarization) {
    return std::visit( [](const auto& pol) { return toStokes(pol); }, polarization);
}

inline math::LocalElectricField toLocalElectricField(const math::Stokes& stokes) {
    return math::toLocalElectricField(stokes);
}

inline math::LocalElectricField toLocalElectricField(const math::LocalElectricField& localElectricField) {
    return localElectricField;
}

inline math::LocalElectricField toLocalElectricField(const Polarization& polarization) {
    return std::visit( [](const auto& pol) { return toLocalElectricField(pol); }, polarization);
}

namespace defaults {

constexpr Polarization polarization = math::Stokes{1.0, 1.0, 0.0, 0.0};

}

}  // namespace rayx::design
