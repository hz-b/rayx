#pragma once

#include <variant>

#include "Core.h"
#include "DatFile.h"

namespace rayx {

enum class SpreadType { HardEdge, SoftEdge, SeparateEnergies };    // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };  // default ET_VALUES

/// Describes a __uniform__ distribution over the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy + m_EnergySpread/2]`.
/// https://en.wikipedia.org/wiki/Discrete_uniform_distribution
struct RAYX_API HardEdge {
    double m_centerEnergy;
    double m_energySpread;

    HardEdge(double centerEnergy, double energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}
};

/// Describes a __normal__ distribution with mean `m_centerEnergy` and standard deviation `m_sigma`.
/// https://en.wikipedia.org/wiki/Normal_distribution
struct RAYX_API SoftEdge {
    double m_centerEnergy;
    double m_sigma;

    SoftEdge(double centerEnergy, double sigma) : m_centerEnergy(centerEnergy), m_sigma(sigma) {}
};

/// Describes a uniform distribution of `m_numberOfEnergies` many discrete energies.
/// These discrete energies lie equidistant within the interval [m_centerEnergy - m_energySpread/2, m_centerEnergy + m_energySpread/2].

/// So you can visualize this distribution, by `m_numberOfEnergies` many spikes in an otherwise-empty diagram.
/// All spikes have the same height (i.e. they have the same probability).
/// A pair of consecutive spikes always has the same distance.
/// The left-most spike is at energy m_centerEnergy - m_energySpread/2, while the right-most spike is at m_centerEnergy + m_energySpread/2.

/// If there is only one spike (i.e. m_numberOfEnergies = 1), then this spike is at `m_centerEnergy`.
struct RAYX_API SeparateEnergies {
    double m_centerEnergy;
    double m_energySpread;
    int m_numberOfEnergies;

    SeparateEnergies(double centerEnergy, double energySpread, int numberOfEnergies)
        : m_centerEnergy(centerEnergy), m_energySpread(energySpread), m_numberOfEnergies(numberOfEnergies) {}
};

using EnergyDistributionVariant = std::variant<DatFile, HardEdge, SoftEdge, SeparateEnergies>;

}  // namespace rayx
