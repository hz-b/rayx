#pragma once

#include <optional>
#include <variant>

#include "Area.h"

namespace rayx {

using ApertureArea            = std::variant<RectangularArea, EllipticalArea, TrapezoidalArea>;
using DiffractiveApertureArea = std::variant<RectangularArea, EllipticalArea>;

struct NonDiffractiveAperture {
    NonDiffractiveAperture(ApertureArea area) : m_area(area) {}

    RAYX_NESTED_PROPERTY(NonDiffractiveAperture, ApertureArea, area);
    RAYX_NESTED_PROPERTY(NonDiffractiveAperture, std::optional<ApertureArea>,
                         beamstopArea);  // uses material of the element
};

struct DiffractiveAperture {
    DiffractiveAperture(DiffractiveApertureArea area) : m_area(area) {}

    RAYX_NESTED_PROPERTY(DiffractiveAperture, DiffractiveApertureArea, area);
    RAYX_NESTED_PROPERTY(DiffractiveAperture, std::optional<ApertureArea>,
                         beamstopArea);  // uses material of the element
};

using Aperture = std::variant<NonDiffractiveAperture, DiffractiveAperture>;

}  // namespace rayx
