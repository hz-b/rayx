#pragma once

#include <glm/glm.hpp>
#include <variant>
#include <vector>

namespace rayx::design {

struct RectangularArea {
    double width  = 1.0;
    double height = 1.0;
};

struct EllipticalArea {
    double diameterX = 0.0;
    double diameterZ = 0.0;
};

struct TrapezoidalArea {
    double topWidth    = 0.0;
    double bottomWidth = 0.0;
    double height      = 0.0;
};

struct ConvexPolygonalArea {
    std::vector<glm::dvec2> points;
};

struct UnlimitedArea {};

using Area = std::variant<RectangularArea, EllipticalArea, TrapezoidalArea, ConvexPolygonalArea, UnlimitedArea>;

}  // namespace rayx::design
