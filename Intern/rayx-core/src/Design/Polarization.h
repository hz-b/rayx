#pragma once

#include <variant>

#include "Math/ElectricField.h"

namespace rayx::design {

using Polarization = std::variant<math::Stokes, math::LocalElectricField>;

namespace defaults {
constexpr Polarization polarization = math::Stokes{1.0, 1.0, 0.0, 0.0};
}

}  // namespace rayx::design
