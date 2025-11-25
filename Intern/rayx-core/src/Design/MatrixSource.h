#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include "Beamline/Node.h"
#include "Beamline/EnergyDistribution.h"
#include "Shader/ElectricField.h"

struct WaveLength {
    double value;  // in nanometers
};

struct ElectroVolt {
    double value;  // in eV
};

using PhotonEnergy = std::variant<WaveLength, ElectroVolt>;

namespace EnergyDistributions {

struct WhiteNoise {
    PhotonEnergy centerEnergy;
    PhotonEnergy energySpread;
};

struct ContinousGaussian {
    PhotonEnergy centerEnergy;
    double sigma;
};

struct DiscreteWhiteNoise {
    PhotonEnergy centerEnergy;
    PhotonEnergy energySpread;
    int numberOfEnergies;
};

struct List {
    std::vector<PhotonEnergy> energies;
    std::optional<std::vector<double>> weights;
    bool continuous;
};

}  // namespace EnergyDistributions

using EnergyDistribution = std::variant<EnergyDistributions::ContinousWhiteNoise, EnergyDistributions::ContinousGaussian,
                                         EnergyDistributions::DiscreteWhiteNoise, EnergyDistributions::List>;

using Polarization = std::variant<ElectricField, Stokes>;

enum class SourceDist { Uniform, Gaussian, Thirds, Circle };  // SourceDist::Thirds represents PixelSource Footprint
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class SigmaType { ST_STANDARD, ST_ACCURATE };
enum class SourceType {
    MatrixSource,
    PointSource,
    CircleSource,
    SimpleUndulatorSource,
    PixelSource,
    DipoleSource,
    InputSource,
};

std::map<SourceType, std::string> SourceTypeToString = {
    {SourceType::MatrixSource, "MatrixSource"},
    {SourceType::PointSource, "PointSource"},
    {SourceType::CircleSource, "CircleSource"},
    {SourceType::SimpleUndulatorSource, "SimpleUndulatorSource"},
    {SourceType::PixelSource, "PixelSource"},
    {SourceType::DipoleSource, "DipoleSource"},
};

std::map<std::string, SourceType> StringToSourceType = {
    {"MatrixSource", SourceType::MatrixSource},
    {"PointSource", SourceType::PointSource},
    {"CircleSource", SourceType::CircleSource},
    {"SimpleUndulatorSource", SourceType::SimpleUndulatorSource},
    {"PixelSource", SourceType::PixelSource},
    {"DipoleSource", SourceType::DipoleSource},
};

std::string getUniqueSourceName(const SourceType sourceType) {
    static size_t counter = 0;
    return std::format("<{}_{}>", SourceTypeToString.at(sourceType), counter++);
}

class SourceBase : public BeamlineNode {
  public:
    SourceBase(const SourceType sourceType) : BeamlineNode(getUniqueSourceName(sourceType)) {}
    SourceBase(std::string name) : BeamlineNode(std::move(name)) {}
    ~SourceBase() = default;

    bool isSource() const override { return true; }
    std::unique_ptr<BeamlineNode> clone() const override; // TODO

    // TODO: these should be implemented in BeamlineNode
    void setName(std::string s) override;
    std::string getName() const override;
    void setPosition(glm::dvec4 p);
    glm::dvec4 getPosition() const override;
    void setOrientation(glm::dmat4x4 o);
    glm::dmat4x4 getOrientation() const override;

  private:
};

class MatrixSource : public SourceBase {
  public:
    MatrixSource() : SourceBase(SourceType::MatrixSource) {}
    MatrixSource(std::string name) : SourceBase(std::move(name)) {}
    ~MatrixSource() = default;

    int numberOfRays;
    double horizontalDivergence;
    double verticalDivergence;
    double width;
    double height;
    double depth;
    Polarization polarization;
};

class PointSource : public SourceBase {
  public:
    PointSource() : SourceBase(SourceType::PointSource) {}
    PointSource(std::string name) : SourceBase(std::move(name)) {}
    ~PointSource() = default;

    int numberOfRays;
    double horizontalDivergence;
    double verticalDivergence;
    double width;
    double height;
    double depth;
    SourceDist horizontalDistribution;
    SourceDist verticalDistribution;
    SourceDist widthDistribution;
    SourceDist heightDistribution;
    Polarization polarization;
    EnergyDistribution energyDistribution;
};

class CircleSource : public SourceBase {
  public:
    CircleSource() : SourceBase(SourceType::CircleSource) {}
    CircleSource(std::string name) : SourceBase(std::move(name)) {}
    ~CircleSource() = default;

    int numberOfRays;
    int numberOfCircles;
    Rad maxOpeningAngle;
    Rad minOpeningAngle;
    Rad deltaOpeningAngle;
    double width;
    double height;
    double depth;
    Polarization polarization;
    EnergyDistribution energyDistribution;
};

class SimpleUndulatorSource : public SourceBase {
  public:
    SimpleUndulatorSource() : SourceBase(SourceType::SimpleUndulatorSource) {}
    SimpleUndulatorSource(std::string name) : SourceBase(std::move(name)) {}
    ~SimpleUndulatorSource() = default;

    int numberOfRays;
    SigmaType sigmaType;
    double undulatorLength;
    double electronSigmaX;
    double electronSigmaXs;
    double electronSigmaY;
    double electronSigmaYs;
    double depth;
    Polarization polarization;
    PhotonEnergy photonEnergy; // this is weird
    EnergyDistribution energyDistribution;
};

class PixelSource : public SourceBase {
  public:
    PixelSource() : SourceBase(SourceType::PixelSource) {}
    PixelSource(std::string name) : SourceBase(std::move(name)) {}
    ~PixelSource() = default;

    int numberOfRays;
    double horizontalDivergence;
    double verticalDivergence;
    double width;
    double height;
    double depth;
    Polarization polarization;
    EnergyDistribution energyDistribution;
};

class DipoleSource : public SourceBase {
  public:
    DipoleSource() : SourceBase(SourceType::DipoleSource) {}
    DipoleSource(std::string name) : SourceBase(std::move(name)) {}
    ~DipoleSource() = default;

    int numberOfRays;
    double bendingRadius;
    ElectronEnergyOrientation electronEnergyOrientation;
    double width;
    double height;
    ElectroVolt electronEnergy;
    ElectroVolt criticalEnergy;
    PhotonEnergy photonEnergy;
    double verticalElectronBeamDivergence;
    double energySpread;
    double horizontalDivergence;
};

class InputSource : public SourceBase {
  public:
    InputSource() : SourceBase(SourceType::InputSource) {}
    InputSource(std::string name) : SourceBase(std::move(name)) {}
    ~InputSource() = default;

    Rays rays;
};

using Source = std::variant<MatrixSource, PointSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;
