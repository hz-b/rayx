#pragma once
#include <array>
#include <glm.hpp>
#include <map>
#include <string>
#include <vector>

#include "Core.h"
#include "Data/xml.h"
#include "DesignElement/DesignSource.h"
#include "EnergyDistribution.h"
#include "Shader/Ray.h"

namespace RAYX {
enum class SpreadType { HardEdge, SoftEdge, SeperateEnergies };    // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };  // default ET_VALUES
enum class SourceDist { Uniform, Gaussian, Thirds, Circle };       // SourceDist::Thirds represents PixelSource Footprint
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class EnergySpreadUnit { EU_PERCENT, EU_eV };
enum class SigmaType { ST_STANDARD, ST_ACCURATE };
struct DesignSource;

class RAYX_API LightSource {
  public:
    LightSource(const DesignSource&);
    virtual ~LightSource() = default;

    // Getter
    Misalignment getMisalignmentParams() const;
    glm::dvec4 getPosition() const { return m_position; }

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    double calcPhotonWavelength(double photonEnergy);

    double selectEnergy() const;
    static glm::dvec3 getDirectionFromAngles(double phi, double psi);
    // get the rays according to specific light source, has to be implemented in
    // each class that inherits from LightSource
    virtual std::vector<Ray> getRays(int thread_count = 0) const = 0;

    std::string m_name;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_EnergyDistribution;
    uint32_t m_numberOfRays;

  protected:
    // Geometric Params

    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

    // in rad:
    double m_horDivergence;  // phi
    double m_verDivergence;  // psi

    glm::dmat4x4 m_orientation = glm::dmat4x4();
    glm::dvec4 m_position = glm::dvec4();

  private:
    // User/Design Parameter
    Misalignment m_misalignmentParams;  // x, y, psi, phi
};

// String conversion functions
const std::map<SpreadType, std::string> SpreadTypeToString = {
    {SpreadType::HardEdge, "HardEdge"}, {SpreadType::SoftEdge, "SoftEdge"}, {SpreadType::SeperateEnergies, "SeperateEnergies"}};

const std::map<std::string, SpreadType> StringToSpreadType = {
    {"HardEdge", SpreadType::HardEdge}, {"SoftEdge", SpreadType::SoftEdge}, {"SeperateEnergies", SpreadType::SeperateEnergies}};

const std::map<EnergyDistributionType, std::string> EnergyDistributionTypeToString = {{EnergyDistributionType::File, "File"},
                                                                                      {EnergyDistributionType::Values, "Values"},
                                                                                      {EnergyDistributionType::Total, "Total"},
                                                                                      {EnergyDistributionType::Param, "Param"}};

const std::map<std::string, EnergyDistributionType> StringToEnergyDistributionType = {{"File", EnergyDistributionType::File},
                                                                                      {"Values", EnergyDistributionType::Values},
                                                                                      {"Total", EnergyDistributionType::Total},
                                                                                      {"Param", EnergyDistributionType::Param}};

const std::map<SourceDist, std::string> SourceDistToString = {
    {SourceDist::Uniform, "Uniform"}, {SourceDist::Gaussian, "Gaussian"}, {SourceDist::Thirds, "Thirds"}, {SourceDist::Circle, "Circle"}};

const std::map<std::string, SourceDist> StringToSourceDist = {
    {"Uniform", SourceDist::Uniform}, {"Gaussian", SourceDist::Gaussian}, {"Thirds", SourceDist::Thirds}, {"Circle", SourceDist::Circle}};

const std::map<ElectronEnergyOrientation, std::string> ElectronEnergyOrientationToString = {
    {ElectronEnergyOrientation::Clockwise, "Clockwise"}, {ElectronEnergyOrientation::Counterclockwise, "Counterclockwise"}};

const std::map<std::string, ElectronEnergyOrientation> StringToElectronEnergyOrientation = {
    {"Clockwise", ElectronEnergyOrientation::Clockwise}, {"Counterclockwise", ElectronEnergyOrientation::Counterclockwise}};

const std::map<EnergySpreadUnit, std::string> EnergySpreadUnitToString = {{EnergySpreadUnit::EU_PERCENT, "Percent"}, {EnergySpreadUnit::EU_eV, "eV"}};

const std::map<std::string, EnergySpreadUnit> StringToEnergySpreadUnit = {{"Percent", EnergySpreadUnit::EU_PERCENT}, {"eV", EnergySpreadUnit::EU_eV}};

const std::map<SigmaType, std::string> SigmaTypeToString = {{SigmaType::ST_STANDARD, "Standard"}, {SigmaType::ST_ACCURATE, "Accurate"}};

const std::map<std::string, SigmaType> StringToSigmaType = {{"Standard", SigmaType::ST_STANDARD}, {"Accurate", SigmaType::ST_ACCURATE}};

}  // namespace RAYX
