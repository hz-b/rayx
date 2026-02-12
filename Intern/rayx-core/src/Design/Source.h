#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "Angle.h"
#include "Distribution.h"
#include "PhotonEnergy.h"
#include "Polarization.h"
#include "Trace/Rays.h"

namespace rayx {

namespace defaults {
constexpr int numRays                      = 100000;
constexpr Polarization polarization        = Stokes(1.0, 1.0, 0.0, 0.0);
constexpr double divergence                = 0.0;
constexpr Distribution2D<Angle> rayAngle   = Distribution2D<Angle>().horizontal(Rad(0.0)).vertical(Rad(0.0));
constexpr Distribution3D<double> rayOrigin = Distribution3D<double>().horizontal(0.0).vertical(0.0).depth(0.0);
}  // namespace defaults

struct PointSource {
    PointSource(Distribution<PhotonEnergy> rayPhotonEnergy) : m_rayPhotonEnergy(rayPhotonEnergy) {}

    RAYX_VALIDATED_PROPERTY(PointSource, int, numRays, detail::validateGreaterZero) = defaults::numRays;
    RAYX_NESTED_PROPERTY(PointSource, Distribution3D<double>, rayOrigin)            = defaults::rayOrigin;
    RAYX_NESTED_PROPERTY(PointSource, Distribution2D<Angle>, rayAngle)              = defaults::rayAngle;
    RAYX_NESTED_PROPERTY(PointSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
    RAYX_NESTED_PROPERTY(PointSource, Polarization, rayPolarization) = defaults::polarization;
};

struct MatrixSource {
    MatrixSource(Distribution<PhotonEnergy> rayPhotonEnergy, double width, double height)
        : m_width(width), m_height(height), m_rayPhotonEnergy(rayPhotonEnergy) {
        validate_width();
        validate_height();
    }

    RAYX_VALIDATED_PROPERTY(MatrixSource, int, numRays, detail::validateGreaterZero) = defaults::numRays;
    RAYX_VALIDATED_PROPERTY(MatrixSource, double, width, detail::validateGreaterEqualZero);
    RAYX_VALIDATED_PROPERTY(MatrixSource, double, height, detail::validateGreaterEqualZero);
    RAYX_PROPERTY(MatrixSource, double, horizontalDivergence) = defaults::divergence;
    RAYX_PROPERTY(MatrixSource, double, verticalDivergence)   = defaults::divergence;
    RAYX_NESTED_PROPERTY(MatrixSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
    RAYX_NESTED_PROPERTY(MatrixSource, Polarization, rayPolarization) = defaults::polarization;
};

struct CircleSource {
    CircleSource(Distribution<PhotonEnergy> rayPhotonEnergy, SeparateValues<double> circleRadius)
        : m_circleRadius(circleRadius), m_rayPhotonEnergy(rayPhotonEnergy) {}

    RAYX_VALIDATED_PROPERTY(CircleSource, int, numRays, detail::validateGreaterZero) = defaults::numRays;
    RAYX_NESTED_PROPERTY(CircleSource, SeparateValues<double>, circleRadius);
    RAYX_PROPERTY(CircleSource, double, divergence) = defaults::divergence;
    RAYX_NESTED_PROPERTY(CircleSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
    RAYX_NESTED_PROPERTY(CircleSource, Polarization, rayPolarization) = defaults::polarization;
};

enum class UndulatorSigmaType { Standard, Accurate };

// TODO: sensible defaults
// TODO: sensible validation
struct SimpleUndulatorSource {
    SimpleUndulatorSource(Distribution<PhotonEnergy> rayPhotonEnergy) : m_rayPhotonEnergy(rayPhotonEnergy) {}

    RAYX_VALIDATED_PROPERTY(SimpleUndulatorSource, int, numRays, detail::validateGreaterZero) = defaults::numRays;
    RAYX_PROPERTY(SimpleUndulatorSource, UndulatorSigmaType, sigmaType)                       = UndulatorSigmaType::Standard;
    RAYX_PROPERTY(SimpleUndulatorSource, double, undulatorLength)                             = 1.0;
    RAYX_PROPERTY(SimpleUndulatorSource, double, electronSigmaX)                              = 0.0;
    RAYX_PROPERTY(SimpleUndulatorSource, double, electronSigmaXs)                             = 0.0;
    RAYX_PROPERTY(SimpleUndulatorSource, double, electronSigmaY)                              = 0.0;
    RAYX_PROPERTY(SimpleUndulatorSource, double, electronSigmaYs)                             = 0.0;
    RAYX_PROPERTY(SimpleUndulatorSource, double, depth)                                       = 0.0;
    RAYX_NESTED_PROPERTY(SimpleUndulatorSource, Polarization, polarization)                   = defaults::polarization;
    RAYX_NESTED_PROPERTY(SimpleUndulatorSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
};

// TODO: sensible defaults
// TODO: sensible validation
struct PixelSource {
    PixelSource(Distribution<PhotonEnergy> rayPhotonEnergy, double width, double height)
        : m_width(width), m_height(height), m_rayPhotonEnergy(rayPhotonEnergy) {
        validate_width();
        validate_height();
    }

    RAYX_VALIDATED_PROPERTY(PixelSource, int, numRays, detail::validateGreaterZero) = defaults::numRays;
    RAYX_PROPERTY(PixelSource, double, horizontalDivergence)                        = defaults::divergence;
    RAYX_PROPERTY(PixelSource, double, verticalDivergence)                          = defaults::divergence;
    RAYX_VALIDATED_PROPERTY(PixelSource, double, width, detail::validateGreaterEqualZero);
    RAYX_VALIDATED_PROPERTY(PixelSource, double, height, detail::validateGreaterEqualZero);
    RAYX_VALIDATED_PROPERTY(PixelSource, double, depth, detail::validateGreaterEqualZero) = 0.0;
    RAYX_NESTED_PROPERTY(PixelSource, Polarization, polarization)                         = defaults::polarization;
    RAYX_NESTED_PROPERTY(PixelSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
};

enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };

// TODO: sensible defaults
// TODO: sensible validation
struct DipoleSource {
    DipoleSource(Distribution<PhotonEnergy> rayPhotonEnergy) : m_rayPhotonEnergy(rayPhotonEnergy) {}

    RAYX_VALIDATED_PROPERTY(DipoleSource, int, numRays, detail::validateGreaterZero)        = defaults::numRays;
    RAYX_PROPERTY(DipoleSource, double, bendingRadius)                                      = 1.0;
    RAYX_PROPERTY(DipoleSource, ElectronEnergyOrientation, electronEnergyOrientation)       = ElectronEnergyOrientation::Clockwise;
    RAYX_VALIDATED_PROPERTY(DipoleSource, double, width, detail::validateGreaterEqualZero)  = 0.0;
    RAYX_VALIDATED_PROPERTY(DipoleSource, double, height, detail::validateGreaterEqualZero) = 0.0;
    RAYX_NESTED_PROPERTY(DipoleSource, ElectronVolt, electronEnergy)                        = ElectronVolt { 3e3 };   // in eV
    RAYX_NESTED_PROPERTY(DipoleSource, ElectronVolt, criticalEnergy)                        = ElectronVolt { 10.0 };  // in eV
    RAYX_NESTED_PROPERTY(DipoleSource, Distribution<PhotonEnergy>, rayPhotonEnergy);
    RAYX_PROPERTY(DipoleSource, double, verticalElectronBeamDivergence) = 0.0;
    RAYX_PROPERTY(DipoleSource, double, energySpread)                   = 0.0;
    RAYX_PROPERTY(DipoleSource, double, horizontalDivergence)           = 0.0;
};

struct InputSource {
    InputSource(Rays rays) : m_rays(std::move(rays)) {}

    RAYX_NESTED_PROPERTY(InputSource, Rays, rays);
    RAYX_NESTED_PROPERTY(InputSource, std::optional<Distribution3D<double>>, rayOrigin) = defaults::rayOrigin;
    RAYX_NESTED_PROPERTY(InputSource, std::optional<Distribution2D<Angle>>, rayAngle)   = defaults::rayAngle;
    RAYX_NESTED_PROPERTY(InputSource, std::optional<Distribution<PhotonEnergy>>, rayPhotonEnergy);
    RAYX_NESTED_PROPERTY(InputSource, std::optional<Polarization>, rayPolarization) = defaults::polarization;
};

using Source = std::variant<PointSource, MatrixSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;

using SourcePtr =
    std::variant<std::shared_ptr<PointSource>, std::shared_ptr<MatrixSource>, std::shared_ptr<CircleSource>, std::shared_ptr<SimpleUndulatorSource>,
                 std::shared_ptr<PixelSource>, std::shared_ptr<DipoleSource>, std::shared_ptr<InputSource>>;

}  // namespace rayx
