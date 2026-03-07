#pragma once

#include <variant>

namespace rayx::detail::model {

struct UnlimitedArea {};

struct RectangularArea {
    double width;
    double height;
};

struct EllipticalArea {
    double horizontalDiameter;
    double verticalDiameter;
};

struct TrapezoidalArea {
    double topWidth;
    double bottomWidth;
    double height;
};

using Area = std::variant<UnlimitedArea, RectangularArea, EllipticalArea, TrapezoidalArea>;

}  // namespace rayx::detail::model
