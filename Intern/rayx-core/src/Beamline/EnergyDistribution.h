#pragma once

#include <variant>

#include "Core.h"
#include "Data/DatFile.h"

namespace RAYX {
/** describes the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy +
 * m_EnergySpread/2]` */
struct RAYX_API EnergyRange {
    float m_centerEnergy;
    float m_energySpread;

    EnergyRange(float centerEnergy, float energySpread);

    float selectEnergy(bool continuous) const;
    float getAverage() const;
};

/**
 * The class EnergyDistribution is contained in LightSources to describe the
 * mathematical distribution from which the energy of the rays are sampled. It
 * can either be a `EnergyRange` being a uniform distribution in some interval,
 * or a `DatFile` which means that the distribution is loaded from a .DAT file.
 */
class RAYX_API EnergyDistribution {
  public:
    EnergyDistribution();  // TODO this default-constructor is required because
                           // LightSource also has one, do we actually want it
                           // though?
    EnergyDistribution(DatFile, bool continuous);
    EnergyDistribution(EnergyRange, bool continuous);

    /** The selectEnergy() function returns one sample from the underlying
     * distribution */
    float selectEnergy() const;
    /** yields the expected value of the distribution */
    float getAverage() const;

  private:
    /** Shows whether the distribution is continuous or discrete */
    bool m_isContinuous;

    /** stores either a DatFile or an EnergyRange, depending on the constructor
     * used to create this */
    std::variant<DatFile, EnergyRange> m_Variant;
};
}  // namespace RAYX
