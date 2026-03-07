#pragma once

#include "Area.h"
#include "Design/Aperture.h"
#include "Model/Aperture.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<ApertureArea> {
    static model::ApertureArea apply(const ApertureArea& area) {
        return std::visit([](const auto& arg) -> model::ApertureArea { return toModel(arg); }, area);
    }
};

template <>
struct ToModel<DiffractiveApertureArea> {
    static model::DiffractiveApertureArea apply(const DiffractiveApertureArea& area) {
        return std::visit([](const auto& arg) -> model::DiffractiveApertureArea { return toModel(arg); }, area);
    }
};

template <>
struct ToModel<NonDiffractiveAperture> {
    static model::NonDiffractiveAperture apply(const NonDiffractiveAperture& aperture) {
        return model::NonDiffractiveAperture{
            .area         = toModel(aperture.area()),
            .beamstopArea = toModel(aperture.beamstopArea()),
        };
    }
};

template <>
struct ToModel<DiffractiveAperture> {
    static model::DiffractiveAperture apply(const DiffractiveAperture& aperture) {
        return model::DiffractiveAperture{
            .area         = toModel(aperture.area()),
            .beamstopArea = toModel(aperture.beamstopArea()),
        };
    }
};

template <>
struct ToModel<Aperture> {
    static model::Aperture apply(const Aperture& aperture) {
        return std::visit([](const auto& arg) -> model::Aperture { return toModel(arg); }, aperture);
    }
};

}  // namespace rayx::detail
