#pragma once

#include <variant>

#include "Core.h"
#include "Data/DatFile.h"

namespace RAYX {
/** describes the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy +
 * m_EnergySpread/2]` */
struct RAYX_API HardEdge {
    double m_centerEnergy;
    double m_energySpread;

    HardEdge(double centerEnergy, double energySpread);

    double selectEnergy() const;
    double getAverage() const;
};

struct RAYX_API SoftEdge {
    double m_centerEnergy;
    double m_sigma;

    SoftEdge(double centerEnergy, double sigma);

    double selectEnergy() const;
    double getAverage() const;
};

struct RAYX_API SeperateEnergies{

  double m_centerEnergy;
  double m_energySpread;
  int m_numberOfEnergies;

  SeperateEnergies(double centerEnergy, double energySpread, int numberOfEnergies);

  double selectEnergy() const;
  double getAverage() const;

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
    EnergyDistribution(SeperateEnergies);

    /** The selectEnergy() function returns one sample from the underlying
     * distribution */
    double selectEnergy() const;
    /** yields the expected value of the distribution */
    double getAverage() const;
    

  private:

    
    /** stores either a DatFile or an HardEdge, depending on the constructor
     * used to create this */
    std::variant<DatFile, HardEdge, SoftEdge, SeperateEnergies> m_Variant;
};
}  // namespace RAYX
