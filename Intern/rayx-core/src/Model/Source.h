#pragma once

#include <optional>
#include <variant>

#include "Distribution.h"
#include "Math/ElectricField.h"
#include "Trace/Rays.h"

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
