#pragma once

#include "Design/Source.h"
#include "Model/Source.h"
#include "ToModelTrait.h"
#include "Distribution.h"
#include "Polarization.h"

namespace rayx::detail {

template <>
struct ToModel<PointSource> {
    static model::PointSource apply(const PointSource& source) {
        return model::PointSource{
            .numRays         = toModel(source.numRays()),
            .rayOrigin       = toModel(source.rayOrigin()),
            .rayAngle        = toModel(source.rayAngle()),
            .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
            .rayPolarization = toModel(source.rayPolarization()),
        };
    }
};

template <>
struct ToModel<MatrixSource> {
    static model::MatrixSource apply(const MatrixSource& source) {
        return model::MatrixSource{
            .numRays              = toModel(source.numRays()),
            .width                = toModel(source.width()),
            .height               = toModel(source.height()),
            .horizontalDivergence = toModel(source.horizontalDivergence()),
            .verticalDivergence   = toModel(source.verticalDivergence()),
            .rayPhotonEnergy      = toModel(source.rayPhotonEnergy()),
            .rayPolarization      = toModel(source.rayPolarization()),
        };
    }
};

template <>
struct ToModel<CircleSource> {
    static model::CircleSource apply(const CircleSource& source) {
        return model::CircleSource{
            .numRays         = toModel(source.numRays()),
            .circleRadius    = toModel(source.circleRadius()),
            .divergence      = toModel(source.divergence()),
            .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
            .rayPolarization = toModel(source.rayPolarization()),
        };
    }
};

template <>
struct ToModel<SimpleUndulatorSource> {
    static model::SimpleUndulatorSource apply(const SimpleUndulatorSource& source) {
        return model::SimpleUndulatorSource{
            .numRays         = toModel(source.numRays()),
            .sigmaType       = toModel(source.sigmaType()),
            .undulatorLength = toModel(source.undulatorLength()),
            .electronSigmaX  = toModel(source.electronSigmaX()),
            .electronSigmaXs = toModel(source.electronSigmaXs()),
            .electronSigmaY  = toModel(source.electronSigmaY()),
            .electronSigmaYs = toModel(source.electronSigmaYs()),
            .depth           = toModel(source.depth()),
            .polarization    = toModel(source.polarization()),
            .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
        };
    }
};

template <>
struct ToModel<PixelSource> {
    static model::PixelSource apply(const PixelSource& source) {
        return model::PixelSource{
            .numRays              = toModel(source.numRays()),
            .width                = toModel(source.width()),
            .height               = toModel(source.height()),
            .horizontalDivergence = toModel(source.horizontalDivergence()),
            .verticalDivergence   = toModel(source.verticalDivergence()),
            .depth                = toModel(source.depth()),
            .polarization         = toModel(source.polarization()),
            .rayPhotonEnergy      = toModel(source.rayPhotonEnergy()),
        };
    }
};

template <>
struct ToModel<DipoleSource> {
    static model::DipoleSource apply(const DipoleSource& source) {
        return model::DipoleSource{
            .numRays                        = toModel(source.numRays()),
            .bendingRadius                  = toModel(source.bendingRadius()),
            .electronEnergyOrientation      = toModel(source.electronEnergyOrientation()),
            .width                          = toModel(source.width()),
            .height                         = toModel(source.height()),
            .electronEnergy                 = toModel(source.electronEnergy()),
            .criticalEnergy                 = toModel(source.criticalEnergy()),
            .rayPhotonEnergy                = toModel(source.rayPhotonEnergy()),
            .verticalElectronBeamDivergence = toModel(source.verticalElectronBeamDivergence()),
            .energySpread                   = toModel(source.energySpread()),
            .horizontalDivergence           = toModel(source.horizontalDivergence()),
        };
    }
};

template <>
struct ToModel<InputSource> {
    static model::InputSource apply(const InputSource& source) {
        return model::InputSource{
            .rays            = source.rays().copy(),  // we have to copy in order to preserve the design layer's ownership of the ray data
            .rayOrigin       = toModel(source.rayOrigin()),
            .rayAngle        = toModel(source.rayAngle()),
            .rayPhotonEnergy = toModel(source.rayPhotonEnergy()),
            .rayPolarization = toModel(source.rayPolarization()),
        };
    }
};

template <>
struct ToModel<Source> {
    static model::Source apply(const Source& source) {
        return std::visit([](const auto& src) -> model::Source { return toModel(src); }, source);
    }
};

}  // namespace rayx::detail
