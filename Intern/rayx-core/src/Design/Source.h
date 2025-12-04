#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "BeamlineNode.h"
#include "Distributions.h"
#include "Rays.h"

namespace defaults {
constexpr int numRays = 100000;
}

struct ArtificialSource {
    int numRays = defaults::numRays;
    ScalarVolumetricDistribution rayOrigin;
    AngularDivergence rayDirection;
    PhotonEnergyDistribution rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization       = defaults::polarization;
};

struct PointSourceParameters {
    int numRays = defaults::numRays;
    double pointSize = 0.0;
    PhotonEnergy rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization = defaults::polarization;
};

struct ProjectorSource {
    int numRays    = defaults::numRays;
    int numCircles = 10;
    ScalarVolumetricDistribution rayOrigin;
    Area projectionArea;
    std::optional<Curvature> projectionAreaCurvature;
    PhotonEnergyDistribution rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization       = defaults::polarization;
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
    std::optional<ScalarVolumetricDistribution> origin;
    std::optional<AngularDivergence> direction;
    std::optional<Polarization> polarization;
    std::optional<ScalarDistribution> energy;
    // TODO: specify which other attributes should be used from the input rays
};

using Source = std::variant<ArtificialSource, ProjectorSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;
