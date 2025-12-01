#pragma once

#include <variant>
#include <optional>
#include <string>
#include <memory>

#include "BeamlineNode.h"
#include "Distributions.h"
#include "Rays.h"

namespace defaults {
constexpr int numRays = 100000;
}

enum class SourceType {
    PointSource,
    CircleSource,
    SimpleUndulatorSource,
    PixelSource,
    DipoleSource,
    InputSource,
};

struct ArtificialSource {
    int numRays = defaults::numRays;
    VolumetricScalarDistribution rayOrigin;
    AngularDivergence rayDirection;
    PhotonEnergyDistribution rayEnergy = defaults::photonEnergy;
    Polarization rayPolarization = defaults::polarization;
};

// TODO: sensible defaults
struct CircleSource {
    static constexpr SourceType sourceType = SourceType::CircleSource;
    int numRays    = defaults::numRays;
    int numCircles = 1;
    Angle maxOpeningAngle;
    Angle minOpeningAngle;
    Angle deltaOpeningAngle;
    double width              = 0.0;
    double height             = 0.0;
    double depth              = 0.0;
    Polarization polarization = defaults::polarization;
    Distribution energy       = defaults::energy;
};

enum class UndulatorSigmaType { Standard, Accurate };

// TODO: sensible defaults
struct SimpleUndulatorSource {
    static constexpr SourceType sourceType = SourceType::SimpleUndulatorSource;
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
    static constexpr SourceType sourceType = SourceType::PixelSource;
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
    static constexpr SourceType sourceType = SourceType::DipoleSource;
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
    static constexpr SourceType sourceType = SourceType::InputSource;
    Rays rays;
    std::optional<VolumetricScalarDistribution> origin;
    std::optional<AngularDivergence> direction;
    std::optional<Polarization> polarization;
    std::optional<ScalarDistribution> energy;
    // TODO: specify which other attributes should be used from the input rays
};

using Source = std::variant<PointSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;

struct SourceNode : BeamlineNode {
    SourceBase() : BeamlineNode(createUniqueSourceName()) {}
    SourceBase(std::string name) : BeamlineNode(std::move(name)) {}
    ~SourceBase() = default;

    bool isSource() const override { return true; }

    std::unique_ptr<BeamlineNode> clone() const override {
        SourceNode clone;
        clone.source = std::visit([](auto&& arg) -> Source { return arg; }, source);
        return std::make_unique<SourceNode>(std::move(clone));
    }

    SourceType sourceType() const {
        return std::visit([]<typename T>(auto&& arg) { return typename T::SourceType; }, source);
    }

    Source source;
};
