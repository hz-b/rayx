#pragma once

#include <numbers>
#include <variant>

namespace rayx::design {

struct Degrees {
    double value;  // in degrees
};

struct Radians {
    double value;  // in radians
};

using Angle = std::variant<Degrees, Radians>;

inline Radians toRadians(const Radians angle) { return angle; }

inline Radians toRadians(const Degrees angle) { return Radians{angle.value * std::numbers::pi / 180.0}; }

inline Radians toRadians(const Angle angle) {
    return std::visit([](auto&& arg) { return toRadians(arg); }, angle);
}

inline Degrees toDegrees(const Degrees angle) { return angle; }

inline Degrees toDegrees(const Radians angle) { return Degrees{angle.value * 180.0 / std::numbers::pi}; }

inline Degrees toDegrees(const Angle angle) {
    return std::visit([](auto&& arg) { return toDegrees(arg); }, angle);
}

namespace literals {

inline Degrees operator"" _deg(long double value) { return Degrees{static_cast<double>(value)}; }

inline Radians operator"" _rad(long double value) { return Radians{static_cast<double>(value)}; }

}  // namespace literals

}  // namespace rayx::design

////////////////////////////////////////////////////////////
// ostream operators
////////////////////////////////////////////////////////////

#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const rayx::design::Degrees degrees) { return os << degrees.value << "(deg)"; }

inline std::ostream& operator<<(std::ostream& os, const rayx::design::Radians radians) { return os << radians.value << "(rad)"; }

inline std::ostream& operator<<(std::ostream& os, const rayx::design::Angle angle) {
    return std::visit([&os](const auto& arg) -> std::ostream& { return os << arg; }, angle);
}

////////////////////////////////////////////////////////////
// std::format specializations
////////////////////////////////////////////////////////////

#include <format>

template <>
struct std::formatter<rayx::design::Degrees> : std::formatter<double> {
    auto format(const rayx::design::Degrees degrees, auto& ctx) const {
        auto out = std::formatter<double>::format(degrees.value, ctx);
        return std::format_to(out, "(deg)");
    }
};

template <>
struct std::formatter<rayx::design::Radians> : std::formatter<double> {
    auto format(const rayx::design::Radians radians, auto& ctx) const {
        auto out = std::formatter<double>::format(radians.value, ctx);
        return std::format_to(out, "(rad)");
    }
};

template <>
struct std::formatter<rayx::design::Angle> : std::formatter<double> {
    auto format(const rayx::design::Angle angle, auto& ctx) const {
        return std::visit(
            [&]<typename T>(const T arg) {
                auto out = std::formatter<double>::format(arg.value, ctx);

                if constexpr (std::is_same_v<T, rayx::design::Degrees>) {
                    return std::format_to(out, "(deg)");
                } else {
                    return std::format_to(out, "(rad)");
                }
            },
            angle);
    }
};
