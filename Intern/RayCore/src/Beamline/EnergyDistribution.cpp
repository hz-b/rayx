#include "EnergyDistribution.h"

#include "Random.h"

namespace RAYX {
EnergyDistribution::EnergyDistribution(DatFile df, bool continuous) : m_isContinuous(continuous), m_Variant(df) {}

EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous) : m_isContinuous(continuous), m_Variant(r) {}

EnergyDistribution::EnergyDistribution() : EnergyDistribution(EnergyRange(100.0, 0.0), false) {}

double EnergyDistribution::selectEnergy() const {
    // the below code calls either DatFile::selectEnergy or
    // EnergyRange::selectEnergy depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> double { return arg.selectEnergy(m_isContinuous); };
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

double EnergyRange::selectEnergy(bool continuous) const {
    if (continuous) {
        return randomDoubleInRange(m_centerEnergy - m_energySpread / 2, m_centerEnergy + m_energySpread / 2);
    } else {
        double arr[3] = {m_centerEnergy - m_energySpread / 2, m_centerEnergy, m_centerEnergy + m_energySpread / 2};
        return arr[randomIntInRange(0, 2)];
    }
}

double EnergyRange::getAverage() const { return m_centerEnergy; }
}  // namespace RAYX
