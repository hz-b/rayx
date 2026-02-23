#include "Source.h"

namespace rayx::detail {

model::PointSource toModel(const PointSource& source) {
    return model::PointSource{
        .numRays         = source.numRays(),
        .rayOrigin       = toModel(source.rayOrigin()),
        .rayAngle        = toModel(source.rayAngle()),
        .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
        .rayPolarization = toModel(source.rayPolarization()),
    };
}

model::MatrixSource toModel(const MatrixSource& source) {
    return model::MatrixSource{
        .numRays              = source.numRays(),
        .width                = source.width(),
        .height               = source.height(),
        .horizontalDivergence = source.horizontalDivergence(),
        .verticalDivergence   = source.verticalDivergence(),
        .rayPhotonEnergy      = toModel(source.rayPhotonEnergy()),
        .rayPolarization      = toModel(source.rayPolarization()),
    };
}

model::CircleSource toModel(const CircleSource& source) {
    return model::CircleSource{
        .numRays         = source.numRays(),
        .circleRadius    = toModel(source.circleRadius()),
        .divergence      = source.divergence(),
        .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
        .rayPolarization = toModel(source.rayPolarization()),
    };
}

model::SimpleUndulatorSource toModel(const SimpleUndulatorSource& source) {
    return model::SimpleUndulatorSource{
        .numRays         = source.numRays(),
        .sigmaType       = source.sigmaType(),
        .undulatorLength = source.undulatorLength(),
        .electronSigmaX  = source.electronSigmaX(),
        .electronSigmaXs = source.electronSigmaXs(),
        .electronSigmaY  = source.electronSigmaY(),
        .electronSigmaYs = source.electronSigmaYs(),
        .depth           = source.depth(),
        .polarization    = toModel(source.polarization()),
        .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
    };
}

model::PixelSource toModel(const PixelSource& source) {
    return model::PixelSource{
        .numRays              = source.numRays(),
        .width                = source.width(),
        .height               = source.height(),
        .horizontalDivergence = source.horizontalDivergence(),
        .verticalDivergence   = source.verticalDivergence(),
        .depth                = source.depth(),
        .polarization         = toModel(source.polarization()),
        .rayPhotonEnergy      = toModel(source.rayPhotonEnergy()),
    };
}

model::DipoleSource toModel(const DipoleSource& source) {
    return model::DipoleSource{
        .numRays                        = source.numRays(),
        .bendingRadius                  = source.bendingRadius(),
        .electronEnergyOrientation      = source.electronEnergyOrientation(),
        .width                          = source.width(),
        .height                         = source.height(),
        .electronEnergy                 = toModel(source.electronEnergy()),
        .criticalEnergy                 = toModel(source.criticalEnergy()),
        .rayPhotonEnergy                = toModel(source.rayPhotonEnergy()),
        .verticalElectronBeamDivergence = source.verticalElectronBeamDivergence(),
        .energySpread                   = source.energySpread(),
        .horizontalDivergence           = source.horizontalDivergence(),
    };
}

model::InputSource toModel(const InputSource& source) {
    return model::InputSource{
        .rays            = source.rays().copy(),  // we have to copy in order to preserve the design layer's ownership of the ray data
        .rayOrigin       = source.rayOrigin() ? std::make_optional(toModel(*source.rayOrigin())) : std::nullopt,
        .rayAngle        = source.rayAngle() ? std::make_optional(toModel(*source.rayAngle())) : std::nullopt,
        .rayPhotonEnergy = source.rayPhotonEnergy() ? std::make_optional(toModel(*source.rayPhotonEnergy())) : std::nullopt,
        .rayPolarization = source.rayPolarization() ? std::make_optional(toModel(*source.rayPolarization())) : std::nullopt,
    };
}

model::Source toModel(const Source& source) {
    return std::visit([](const auto& src) -> model::Source { return toModel(src); }, source);
}

}  // namespace rayx::detail
