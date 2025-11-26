#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include "Beamline/EnergyDistribution.h"
#include "Beamline/Node.h"
#include "Shader/ElectricField.h"

namespace defaults {
constexpr int numRays = 100000;
}

/*
 * TODO: remove all of this
 */

enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class UndulatorSigmaType { Standard, Accurate };

enum class SourceType {
    MatrixSource,
    PointSource,
    CircleSource,
    SimpleUndulatorSource,
    PixelSource,
    DipoleSource,
    InputSource,
};

/*
 * photon energy: wavelength in nm, energy in eV
 */

struct WaveLength {
    double value;  // in nanometers
};

struct ElectronVolt {
    double value;  // in eV
};

WaveLength toWaveLength(const WaveLength wavelength) { return wavelength; }

WaveLength toWaveLength(const ElectronVolt energy) { return WaveLength{INV_NM_TO_EVOLT / energy.value}; }

WaveLength toWaveLength(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toWaveLength(arg); }, energy);
}

ElectronVolt toElectronVolt(const ElectronVolt energy) { return energy; }

ElectronVolt toElectronVolt(const WaveLength wavelength) { return ElectronVolt{INV_NM_TO_EVOLT / wavelength.value}; }

ElectronVolt toElectronVolt(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toElectronVolt(arg); }, energy);
}

using PhotonEnergy       = std::variant<WaveLength, ElectronVolt>;
using PhotonEnergyVector = std::variant<std::vector<WaveLength>, std::vector<ElectronVolt>>;

/*
 * angle: degrees, radians
 */

struct Degrees {
    double value;  // in degrees
};

struct Radians {
    double value;  // in radians
};

using Angle = std::variant<Degrees, Radians>;

Radians toRadians(const Radians angle) { return angle; }

Radians toRadians(const Degrees angle) { return Radians{angle.value * glm::pi<double>() / 180.0}; }

Radians toRadians(const Angle angle) {
    return std::visit([](auto&& arg) { return toRadians(arg); }, angle);
}

Degrees toDegrees(const Degrees angle) { return angle; }

Degrees toDegrees(const Radians angle) { return Degrees{angle.value * 180.0 / glm::pi<double>()}; }

Degrees toDegrees(const Angle angle) {
    return std::visit([](auto&& arg) { return toDegrees(arg); }, angle);
}

/*
 * distribution: uniform, gaussian, baked
 */

template <typename T>
struct RangeSampler {
    T halfRange                   = T();
    std::optional<int> numSamples = std::nullopt;
};

template <typename T>
struct UniformDistribution {
    T center = T();
    RangeSampler sampler;
};

template <typename T>
struct BrownianDistribution {
    T center            = T();
    T standardDeviation = T();
    RangeSampler sampler;
};

template <typename T>
struct GaussianDistriubution {
    PhotonEnergy mean;
    PhotonEnergy standardDeviation;
    std::optional<RangeSampler> sampler;
};

template <typename T>
struct BakedDistribution {
    PhotonEnergyVector samples;
    std::optional<std::vector<double>> weights;
    bool interpolate = false;
};

template <typename T>
using Distribution        = std::variant<UniformDistribution<T>, BrownianDistribution<T>, GaussianDistribution<T>, BakedDistribution<T>>;
using ScalarDistribution  = Distribution<double>;
using AngularDistribution = Distribution<Angle>;

namespace defaults {
constexpr ScalarDistribution energy = UniformDistribution<double>{.center = 1.0 /* infrared */};
}

template <typename T>
struct CircularDistribution {
    Distribution<T> radiusDistribution;
};
using ScalarCircularDistribution  = CircularDistribution<double>;
using AngularCircularDistribution = CircularDistribution<Angle>;

template <typename T>
struct RectangularDistribution {
    Distribution<T> widthDistribution;
    Distribution<T> heightDistribution;
};
using ScalarRectangularDistribution  = RectangularDistribution<double>;
using AngularRectangularDistribution = RectangularDistribution<Angle>;

template <typename T>
using AreaDistribution        = std::variant<CircularDistribution<T>, RectangularDistribution<T>>;
using AreaScalarDistribution  = AreaDistribution<double>;
using AreaAngularDistribution = AreaDistribution<Angle>;

template <typename T>
struct SphericalDistribution {
    Distribution<T> radiusDistribution;
};
using ScalarSphericalDistribution = SphericalDistribution<double>;

template <typename T>
struct CylindricalDistribution {
    Distribution<T> radiusDistribution;
    Distribution<T> depthDistribution;
};
using ScalarCylindricalDistribution = CylindricalDistribution<double>;

template <typename T>
struct CuboidalDistribution {
    Distribution<T> widthDistribution;
    Distribution<T> heightDistribution;
    Distribution<T> depthDistribution;
};
using ScalarCuboidalDistribution = CuboidalDistribution<double>;

template <typename T>
using VolumetricDistribution       = std::variant<CuboidalDistribution<T>, CylindricalDistribution<T>, SphericalDistribution<T>>;
using VolumetricScalarDistribution = VolumetricDistribution<double>;

/*
 * divergence: collimated, individual
 */

// ray direction is proportional to ray origin
struct CollimatedAngularDivergence {
    Angle horDivergence;
    Angle verDivergence;
};

struct IndividualAngularDivergence {
    AreaAngularDistribution distribution;
};

using AngularDivergence = std::variant<CollimatedAngularDivergence, IndividualAngularDivergence>;

/*
 * polarization: stokes, electric field
 */

using Polarization = std::variant<Stokes, ElectricField>;

namespace defaults {
constexpr Polarization polarization = Stokes{1.0, 0.0, 0.0, 0.0};
}

/*
 * rotation: matrix, base convention, axis-angle
 */

// essentially a quaternion rotation (glm::dquat)
struct RotationAroundAxis {
    Angle angle;
    glm::dvec3 axis;
};

struct RotationBase {
    glm::dvec3 right;
    glm::dvec3 up;
    glm::dvec3 forward;
};

using Rotation = std::variant<glm::dmat3, RotationBase, RotationAroundAxis>;

RotationAroundAxis toRotationAroundAxis(const RotationAroundAxis& rotation) { return rotation; }

RotationAroundAxis toRotationAroundAxis(const RotationBase& rotation) {
    const auto quat = glm::quat_cast(glm::dmat3(rotation.right, rotation.up, rotation.forward));
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

RotationAroundAxis toRotationAroundAxis(const glm::dmat3& rotation) {
    const auto quat = glm::quat_cast(rotation);
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

RotationAroundAxis toRotationAroundAxis(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationAroundAxis(arg); }, rotation);
}

RotationBase toRotationBase(const RotationBase& rotation) { return rotation; }

RotationBase toRotationBase(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    const auto mat  = glm::mat3_cast(quat);
    return RotationBase{
        .right   = mat[0],
        .up      = mat[1],
        .forward = mat[2],
    };
}

RotationBase toRotationBase(const glm::dmat3& rotation) {
    return RotationBase{
        .right   = rotation[0],
        .up      = rotation[1],
        .forward = rotation[2],
    };
}

RotationBase toRotationBase(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationBase(arg); }, rotation);
}

glm::dmat3 toRotationMatrix(const glm::dmat3& rotation) { return rotation; }

glm::dmat3 toRotationMatrix(const RotationBase& rotation) { return glm::dmat3(rotation.right, rotation.up, rotation.forward); }

glm::dmat3 toRotationMatrix(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    return glm::mat3_cast(quat);
}

glm::dmat3 toRotationMatrix(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationMatrix(arg); }, rotation);
}

struct BeamlineNode {
    BeamlineNode(std::string name)
        : name(std::move(name)), position(0.0), rotation(RotationBase{glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0), glm::dvec3(0, 0, 1)}) {}

    std::string name;
    glm::dvec3 position;
    Rotation rotation;
};

/*
 * sources
 */

struct PointSource {
    using SourceType = SourceType::PointSource;
    int numRays = defaults::numRays;
    VolumetricScalarDistribution origin;
    AngularDivergence direction;
    Polarization polarization = defaults::polarization;
    ScalarDistribution energy = defaults::energy;
};

// TODO: sensible defaults
struct CircleSource {
    using SourceType = SourceType::CircleSource;
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

// TODO: sensible defaults
struct SimpleUndulatorSource {
    using SourceType = SourceType::SimpleUndulatorSource;
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
    using SourceType = SourceType::PixelSource;
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

// TODO: sensible defaults
struct DipoleSource {
    using SourceType = SourceType::DipoleSource;
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
    using SourceType = SourceType::InputSource;
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
    Source source;

    SourceType sourceType() const {
        return std::visit([]<typename T>(auto&& arg) { return typename T::SourceType; }, source);
    }
};
