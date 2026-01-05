#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "BeamlineNode.h"
#include "Distributions.h"
#include "Rays.h"

namespace defaults {
constexpr int numRays                                             = 100000;
constexpr Cube<UniformDistribution<double>> matrixSourceRayOrigin = {{0.0, 2.0, 100}, {0.0, 2.0, 100}, {0.0, 0.0, 1}};
constexpr SeparateValues<double> circleSourceRadius               = {.center = 0.0, .range = 1.0, .numValues = 10};
}  // namespace defaults

struct PointSource {
    int numRays = defaults::numRays;
    Cube<Distribution<double>> rayOrigin;
    Rect<Distribution<Angle>> rayAngle;
    Distribution<PhotonEnergy> rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization         = defaults::polarization;
};

struct MatrixSource {
    int numRays = defaults::numRays;
    // bool multipleRaysPerOrigin = true;
    double width                         = 2.0;
    double height                        = 2.0;
    double horizontalDivergence          = 0.0;
    double verticalDivergence            = 0.0;
    Distribution<PhotonEnergy> rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization         = defaults::polarization;
};

struct CircleSource {
    int numRays                   = defaults::numRays;
    SeparateValues<double> radius = defaults::circleSourceRadius;
    Rect<Distribution<Angle>> rayAngle;
    Distribution<PhotonEnergy> rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization         = defaults::polarization;
};

enum class UndulatorSigmaType { Standard, Accurate };

// TODO: sensible defaults
struct SimpleUndulatorSource {
    int numRays                  = defaults::numRays;
    UndulatorSigmaType sigmaType = UndulatorSigmaType::Standard;
    double undulatorLength       = 1.0;
    double electronSigmaX        = 0.0;
    double electronSigmaXs       = 0.0;
    double electronSigmaY        = 0.0;
    double electronSigmaYs       = 0.0;
    double depth                 = 0.0;
    Polarization polarization    = defaults::polarization;
    PhotonEnergy photonEnergy    = 1.0;  // TODO: this is weird, because PhotonEnergy is potentially redundant to energy
    Distribution energy          = defaults::energy;
};

// TODO: sensible defaults
struct PixelSource {
    int numRays = defaults::numRays;
    // TODO: change to AngularDivergence ? depends on how PixelSource works
    double horizontalDivergenc = 0.0;
    double verticalDivergence  = 0.0;
    // TODO: change to VolumetricScalarDistribution ? depends on how PixelSource works
    double width              = 0.0;
    double height             = 0.0;
    double depth              = 0.0;
    Polarization polarization = defaults::polarization;
    Distribution energy       = defaults::energy;
};

enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };

// TODO: sensible defaults
struct DipoleSource {
    int numRays                                         = defaults::numRays;
    double bendingRadius                                = 1.0;
    ElectronEnergyOrientation electronEnergyOrientation = ElectronEnergyOrientation::Clockwise;
    double width                                        = 0.0;
    double height                                       = 0.0;
    ElectronVolt electronEnergy                         = ElectronVolt{3e3};   // in eV
    ElectronVolt criticalEnergy                         = ElectronVolt{10.0};  // in eV
    PhotonEnergy photonEnergy                           = WaveLength{1.24};    // in nm
    double verticalElectronBeamDivergence               = 0.0;
    double energySpread                                 = 0.0;
    double horizontalDivergence                         = 0.0;
};

struct InputSource {
    Rays rays;
    std::optional<Cube<Distribution<double>>> rayOrigin;
    std::optional<Rect<Distribution<Angle>>> rayAngle;
    std::optional<Distribution<PhotonEnergy>> rayEnergy;
    std::optional<Polarization> rayPolarization;
};

using Source = std::variant<PointSource, MatrixSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;
