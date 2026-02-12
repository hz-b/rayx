#pragma once

#include <numbers>
#include <variant>

#include "Property.h"

////////////////////////////////////////////////////////////
// type definitions
////////////////////////////////////////////////////////////

namespace rayx {

struct Deg {
    constexpr Deg() : m_value(0.0) {}
    constexpr Deg(double value) : m_value(value) {}

    RAYX_PROPERTY(Deg, double, value);
};

struct Rad {
    constexpr Rad() : m_value(0.0) {}
    constexpr Rad(double value) : m_value(value) {}

    RAYX_PROPERTY(Rad, double, value);
};

using Angle = std::variant<Deg, Rad>;

}  // namespace rayx

////////////////////////////////////////////////////////////
// conversion functions
////////////////////////////////////////////////////////////

namespace rayx {

inline Rad toRad(const Rad rad) { return rad; }

inline Rad toRad(const Deg deg) { return Rad{deg.value() * std::numbers::pi / 180.0}; }

inline Rad toRad(const Angle angle) {
    return std::visit([](auto&& arg) { return toRad(arg); }, angle);
}

inline Deg toDeg(const Rad rad) { return Deg{rad.value() * 180.0 / std::numbers::pi}; }

inline Deg toDeg(const Deg deg) { return deg; }

inline Deg toDeg(const Angle angle) {
    return std::visit([](auto&& arg) { return toDeg(arg); }, angle);
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// user-defined literals
////////////////////////////////////////////////////////////

namespace rayx::literals {

inline Deg operator"" _deg(long double value) { return Deg{static_cast<double>(value)}; }

inline Rad operator"" _rad(long double value) { return Rad{static_cast<double>(value)}; }

}  // namespace rayx::literals

////////////////////////////////////////////////////////////
// std::ostream operator<< overloads
////////////////////////////////////////////////////////////

#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const rayx::Deg degrees) { return os << degrees.value() << "(deg)"; }

inline std::ostream& operator<<(std::ostream& os, const rayx::Rad radians) { return os << radians.value() << "(rad)"; }

inline std::ostream& operator<<(std::ostream& os, const rayx::Angle angle) {
    return std::visit([&os](const auto& arg) -> std::ostream& { return os << arg; }, angle);
}

////////////////////////////////////////////////////////////
// std::formatter specializations
////////////////////////////////////////////////////////////

// TODO: simplify
// TODO: find a way to avoid code duplication here

#include <format>

template <>
struct std::formatter<rayx::Deg> : std::formatter<double> {
    auto format(const rayx::Deg degrees, auto& ctx) const {
        auto out = std::formatter<double>::format(degrees.value, ctx);
        return std::format_to(out, "(deg)");
    }
};

template <>
struct std::formatter<rayx::Rad> : std::formatter<double> {
    auto format(const rayx::Rad radians, auto& ctx) const {
        auto out = std::formatter<double>::format(radians.value, ctx);
        return std::format_to(out, "(rad)");
    }
};

template <>
struct std::formatter<rayx::Angle> : std::formatter<double> {
    auto format(const rayx::Angle angle, auto& ctx) const {
        return std::visit(
            [&]<typename T>(const T arg) {
                auto out = std::formatter<double>::format(arg.value, ctx);

                if constexpr (std::is_same_v<T, rayx::Deg>) {
                    return std::format_to(out, "(deg)");
                } else {
                    return std::format_to(out, "(rad)");
                }
            },
            angle);
    }
};
