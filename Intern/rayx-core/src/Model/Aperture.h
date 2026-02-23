#pragma once

#include <variant>

#include "Area.h"
#include "Design/Aperture.h"
#include "ToModel.h"

namespace rayx::detail::model {

using ApertureArea            = std::variant<model::RectangularArea, model::EllipticalArea, model::TrapezoidalArea>;
using DiffractiveApertureArea = std::variant<model::RectangularArea, model::EllipticalArea>;

struct NonDiffractiveAperture {
    model::ApertureArea area;
    std::optional<model::ApertureArea> beamstopArea;
};

struct DiffractiveAperture {
    model::DiffractiveApertureArea area;
    std::optional<model::ApertureArea> beamstopArea;
};

using Aperture = std::variant<model::NonDiffractiveAperture, model::DiffractiveAperture>;

}  // namespace rayx::detail::model

namespace rayx::detail {

model::NonDiffractiveAperture toModel(const NonDiffractiveAperture& aperture);
model::DiffractiveAperture toModel(const DiffractiveAperture& aperture);
model::Aperture toModel(const Aperture& aperture);

}  // namespace rayx::detail
