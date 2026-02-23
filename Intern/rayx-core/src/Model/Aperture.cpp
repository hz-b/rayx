#include "Aperture.h"

namespace rayx::detail {

namespace {

model::ApertureArea toModel(const ApertureArea& area) {
    return std::visit([](const auto& a) -> model::ApertureArea { return detail::toModel(a); }, area);
}

model::DiffractiveApertureArea toModel(const DiffractiveApertureArea& area) {
    return std::visit([](const auto& a) -> model::DiffractiveApertureArea { return detail::toModel(a); }, area);
}

}  // namespace

model::NonDiffractiveAperture toModel(const NonDiffractiveAperture& aperture) {
    return model::NonDiffractiveAperture{
        .area         = toModel(aperture.area()),
        .beamstopArea = aperture.beamstopArea() ? std::make_optional(toModel(*aperture.beamstopArea())) : std::nullopt,
    };
}

model::DiffractiveAperture toModel(const DiffractiveAperture& aperture) {
    return model::DiffractiveAperture{
        .area         = toModel(aperture.area()),
        .beamstopArea = aperture.beamstopArea() ? std::make_optional(toModel(*aperture.beamstopArea())) : std::nullopt,
    };
}

model::Aperture toModel(const Aperture& aperture) {
    return std::visit([](const auto& a) -> model::Aperture { return toModel(a); }, aperture);
}

}  // namespace rayx::detail
