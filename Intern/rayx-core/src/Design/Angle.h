#pragma once

#include <variant>
#include <numbers>

namespace rayx::design {

struct Degrees {
    double value;  // in degrees
};

struct Radians {
    double value;  // in radians
};

using Angle = std::variant<Degrees, Radians>;

Radians toRadians(const Radians angle) { return angle; }

Radians toRadians(const Degrees angle) { return Radians{angle.value * std::numbers::pi / 180.0}; }

Radians toRadians(const Angle angle) {
    return std::visit([](auto&& arg) { return toRadians(arg); }, angle);
}

Degrees toDegrees(const Degrees angle) { return angle; }

Degrees toDegrees(const Radians angle) { return Degrees{angle.value * 180.0 / std::numbers::pi}; }

Degrees toDegrees(const Angle angle) {
    return std::visit([](auto&& arg) { return toDegrees(arg); }, angle);
}

// TODO: write ostream operators and compatibility for std::format/std::print
// std::ostream operator<<(std::ostream& os, const Degrees& angle) {
//     return os << angle.value << "(deg)";
// }

namespace literals {

Degrees operator"" _deg(long double value) { return Degrees{static_cast<double>(value)}; }

Radians operator"" _rad(long double value) { return Radians{static_cast<double>(value)}; }

}  // namespace literals

}  // namespace rayx::design
