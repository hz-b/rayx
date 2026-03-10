#pragma once

#include "Design/Area.h"
#include "Model/Area.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<UnlimitedArea> {
    static model::UnlimitedArea apply(const UnlimitedArea&) { return model::UnlimitedArea{}; }
};

template <>
struct ToModel<RectangularArea> {
    static model::RectangularArea apply(const RectangularArea& area) {
        return model::RectangularArea{
            .width  = area.width(),
            .height = area.height(),
        };
    }
};

template <>
struct ToModel<EllipticalArea> {
    static model::EllipticalArea apply(const EllipticalArea& area) {
        return model::EllipticalArea{
            .horizontalDiameter = area.horizontalDiameter(),
            .verticalDiameter   = area.verticalDiameter(),
        };
    }
};

template <>
struct ToModel<TrapezoidalArea> {
    static model::TrapezoidalArea apply(const TrapezoidalArea& area) {
        return model::TrapezoidalArea{
            .topWidth    = area.topWidth(),
            .bottomWidth = area.bottomWidth(),
            .height      = area.height(),
        };
    }
};

template <>
struct ToModel<Area> {
    static model::Area apply(const Area& area) {
        return std::visit([](const auto& a) -> model::Area { return toModel(a); }, area);
    }
};

}  // namespace rayx::detail
