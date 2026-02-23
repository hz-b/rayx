#include "Area.h"

namespace rayx::detail {

model::UnlimitedArea toModel(const UnlimitedArea&) { return model::UnlimitedArea{}; }

model::RectangularArea toModel(const RectangularArea& area) {
    return model::RectangularArea{
        .width  = area.width(),
        .height = area.height(),
    };
}

model::EllipticalArea toModel(const EllipticalArea& area) {
    return model::EllipticalArea{
        .horizontalDiameter = area.horizontalDiameter(),
        .verticalDiameter   = area.verticalDiameter(),
    };
}

model::TrapezoidalArea toModel(const TrapezoidalArea& area) {
    return model::TrapezoidalArea{
        .topWidth    = area.topWidth(),
        .bottomWidth = area.bottomWidth(),
        .height      = area.height(),
    };
}

model::Area toModel(const Area& area) {
    return std::visit([](const auto& a) -> model::Area { return toModel(a); }, area);
}

}  // namespace rayx::detail
