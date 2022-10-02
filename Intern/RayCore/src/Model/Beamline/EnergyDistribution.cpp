#include "EnergyDistribution.h"

#include <ctime>
namespace RAYX {
std::mt19937 EnergyDistribution::s_rng(time(nullptr));

EnergyDistribution::EnergyDistribution(DatFile df, bool continuous) : m_isContinuous(continuous), m_Variant(df) {}

EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous) : m_isContinuous(continuous), m_Variant(r) {}

EnergyDistribution::EnergyDistribution() : EnergyDistribution(EnergyRange(100.0, 0.0), false) {}

double EnergyDistribution::selectEnergy() const {
    // the below code calls either DatFile::selectEnergy or
    // EnergyRange::selectEnergy depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> double { return arg.selectEnergy(s_rng, m_isContinuous); };
    return std::visit(func, m_Variant);
}

double EnergyDistribution::getAverage() const {
    // the below code calls either DatFile::getAverage or
    // EnergyRange::getAverage depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> double { return arg.getAverage(); };
    return std::visit(func, m_Variant);
}

// EnergyRange impls

EnergyRange::EnergyRange(double centerEnergy, double energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}

double EnergyRange::selectEnergy(std::mt19937& rng, bool continuous) const {
    if (continuous) {
        double percentage = ((double)rng()) / std::mt19937::max();  // in [0, 1]
        return m_centerEnergy + ((percentage - 0.5) * m_energySpread);
    } else {
        double arr[3] = {m_centerEnergy - m_energySpread / 2, m_centerEnergy, m_centerEnergy + m_energySpread / 2};
        return arr[rng() % 3];  // this chooses a random of the 3 elements of
                                // `arr` above
    }
}

double EnergyRange::getAverage() const { return m_centerEnergy; }
}  // namespace RAYX