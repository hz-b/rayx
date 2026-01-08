#pragma once

#include <variant>

#include "ElectricField.h"
#include "Stokes.h"

namespace rayx::design {

using Polarization = std::variant<Stokes, LocalElectricField>;

namespace defaults {
constexpr Polarization polarization = Stokes{1.0, 1.0, 0.0, 0.0};
}

}  // namespace rayx::design
