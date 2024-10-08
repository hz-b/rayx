#pragma once

#include <variant>

#include "Core.h"
#include "DatFile.h"

namespace RAYX {

/// Describes a __uniform__ distribution over the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy + m_EnergySpread/2]`.
/// https://en.wikipedia.org/wiki/Discrete_uniform_distribution
struct RAYX_API HardEdge {
    double m_centerEnergy;
    double m_energySpread;

    HardEdge(double centerEnergy, double energySpread);

    double selectEnergy() const;
};

/// Describes a __normal__ distribution with mean `m_centerEnergy` and standard deviation `m_sigma`.
/// https://en.wikipedia.org/wiki/Normal_distribution
struct RAYX_API SoftEdge {
    double m_centerEnergy;
    double m_sigma;

    SoftEdge(double centerEnergy, double sigma);

    double selectEnergy() const;
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

    SeparateEnergies(double centerEnergy, double energySpread, int numberOfEnergies);

    double selectEnergy() const;
};

/**
 * The class EnergyDistribution is contained in LightSources to describe the
 * mathematical distribution from which the energy of the rays are sampled. It
 * can either be a `HardEdge` being a uniform distribution in some interval,
 * or a `DatFile` which means that the distribution is loaded from a .DAT file.
 */
class RAYX_API EnergyDistribution {
  public:
    EnergyDistribution();  // TODO this default-constructor is required because
                           // LightSource also has one, do we actually want it
                           // though?
    EnergyDistribution(DatFile);
    EnergyDistribution(HardEdge);
    EnergyDistribution(SoftEdge);
    EnergyDistribution(SeparateEnergies);

    // The selectEnergy() function returns one sample from the underlying distribution.
    // The energy is returned in eV.
    double selectEnergy() const;

  private:
    // Stores either a DatFile, or a HardEdge, or ... etc.
    // The object within m_Variant is the *actual* energy distribution.
    std::variant<DatFile, HardEdge, SoftEdge, SeparateEnergies> m_Variant;
};
}  // namespace RAYX
