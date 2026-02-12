#pragma once

#include <glm/glm.hpp>
#include <variant>
#include <vector>

#include "Property.h"

namespace rayx {

struct RectangularArea {
    RectangularArea(double width, double height) {
        this->width(width);
        this->height(height);
    }

    RAYX_VALIDATED_PROPERTY(RectangularArea, double, width, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(RectangularArea, double, height, detail::validateGreaterZero);
};

struct EllipticalArea {
    EllipticalArea(double horizontalDiameter, double verticalDiameter) {
        this->horizontalDiameter(horizontalDiameter);
        this->verticalDiameter(verticalDiameter);
    }

    RAYX_VALIDATED_PROPERTY(EllipticalArea, double, horizontalDiameter, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(EllipticalArea, double, verticalDiameter, detail::validateGreaterZero);
};

struct TrapezoidalArea {
    TrapezoidalArea(double topWidth, double bottomWidth, double height) {
        this->topWidth(topWidth);
        this->bottomWidth(bottomWidth);
        this->height(height);
    }

    RAYX_VALIDATED_PROPERTY(TrapezoidalArea, double, topWidth, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(TrapezoidalArea, double, bottomWidth, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(TrapezoidalArea, double, height, detail::validateGreaterZero);
};

// TODO: implement
// struct ConvexPolygonalArea {
//     std::vector<glm::dvec2> points;
// };

struct UnlimitedArea {};

using Area = std::variant<RectangularArea, EllipticalArea, TrapezoidalArea, UnlimitedArea>;

}  // namespace rayx
