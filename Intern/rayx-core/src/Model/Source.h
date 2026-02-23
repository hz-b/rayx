#pragma once

#include "Design/Source.h"
#include "Distribution.h"

namespace rayx::detail::model {

struct PointSource {
    int numRays;
    model::Distribution3D rayOrigin;
    model::Distribution2D rayAngle;
    model::Distribution rayPhotonEnergy;
    LocalElectricField rayPolarization;
};

struct MatrixSource {
    int numRays;
    double width;
    double height;
    double horizontalDivergence;
    double verticalDivergence;
    model::Distribution rayPhotonEnergy;
    LocalElectricField rayPolarization;
};

struct CircleSource {
    int numRays;
    model::SeparateValues circleRadius;
    double divergence;
    model::Distribution rayPhotonEnergy;
    LocalElectricField rayPolarization;
};

struct SimpleUndulatorSource {
    int numRays;
    UndulatorSigmaType sigmaType;
    double undulatorLength;
    double electronSigmaX;
    double electronSigmaXs;
    double electronSigmaY;
    double electronSigmaYs;
    double depth;
    LocalElectricField polarization;
    model::Distribution rayPhotonEnergy;
};

struct PixelSource {
    int numRays;
    double width;
    double height;
    double horizontalDivergence;
    double verticalDivergence;
    double depth;
    LocalElectricField polarization;
    model::Distribution rayPhotonEnergy;
};

struct DipoleSource {
    int numRays;
    double bendingRadius;
    ElectronEnergyOrientation electronEnergyOrientation;
    double width;
    double height;
    double electronEnergy;
    double criticalEnergy;
    model::Distribution rayPhotonEnergy;
    double verticalElectronBeamDivergence;
    double energySpread;
    double horizontalDivergence;
};

struct InputSource {
    Rays rays;
    std::optional<model::Distribution3D> rayOrigin;
    std::optional<model::Distribution2D> rayAngle;
    std::optional<model::Distribution> rayPhotonEnergy;
    std::optional<LocalElectricField> rayPolarization;
};

using Source = std::variant<PointSource, MatrixSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;

}  // namespace rayx::detail::model

namespace rayx::detail {

// all the overloads for sources are put here, to avoid an extra copy, when calling toModel with an already unwrapped Source
model::PointSource toModel(const PointSource& source);
model::MatrixSource toModel(const MatrixSource& source);
model::CircleSource toModel(const CircleSource& source);
model::SimpleUndulatorSource toModel(const SimpleUndulatorSource& source);
model::PixelSource toModel(const PixelSource& source);
model::DipoleSource toModel(const DipoleSource& source);
model::InputSource toModel(const InputSource& source);
model::Source toModel(const Source& source);

}  // namespace rayx::detail
