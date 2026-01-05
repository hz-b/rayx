#pragma once

#include <optional>
#include <variant>

#include "Area.h"

namespace rayx {

using ApertureArea            = std::variant<RectangularArea, EllipticalArea, TrapezoidalArea, ConvexPolygonalArea>;
using DiffractiveApertureArea = std::variant<RectangularArea, EllipticalArea>;

struct Aperture {
    ApertureArea area;
    std::optional<ApertureArea> beamstopArea;  // uses material of the element
};

struct DiffractiveAperture {
    DiffractiveApertureArea area;
    std::optional<ApertureArea> beamstopArea;  // uses material of the element
};

using Aperture = std::variant<Aperture, DiffractiveAperture>;

}  // namespace rayx
