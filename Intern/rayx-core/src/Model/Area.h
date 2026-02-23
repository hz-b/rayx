#pragma once

#include "Design/Area.h"
#include "ToModel.h"

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

namespace rayx::detail {

model::UnlimitedArea toModel(const UnlimitedArea& area);
model::RectangularArea toModel(const RectangularArea& area);
model::EllipticalArea toModel(const EllipticalArea& area);
model::TrapezoidalArea toModel(const TrapezoidalArea& area);
model::Area toModel(const Area& area);

}  // namespace rayx::detail
