#pragma once

#include <glm/glm.hpp>
#include <variant>

namespace rayx {

struct Degrees {
    double value;  // in degrees
};

struct Radians {
    double value;  // in radians
};

using Angle = std::variant<Degrees, Radians>;

Radians toRadians(const Radians angle) { return angle; }

Radians toRadians(const Degrees angle) { return Radians{angle.value * glm::pi<double>() / 180.0}; }

Radians toRadians(const Angle angle) {
    return std::visit([](auto&& arg) { return toRadians(arg); }, angle);
}

Degrees toDegrees(const Degrees angle) { return angle; }

Degrees toDegrees(const Radians angle) { return Degrees{angle.value * 180.0 / glm::pi<double>()}; }

Degrees toDegrees(const Angle angle) {
    return std::visit([](auto&& arg) { return toDegrees(arg); }, angle);
}

namespace literals {

Degrees operator"" _deg(long double value) { return Degrees{static_cast<double>(value)}; }

Radians operator"" _rad(long double value) { return Radians{static_cast<double>(value)}; }

}  // namespace literals

}  // namespace rayx
