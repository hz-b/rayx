#include "EnergyDistribution.h"

#include "Random.h"

namespace RAYX {
EnergyDistribution::EnergyDistribution(DatFile df, bool continuous) : m_isContinuous(continuous), m_Variant(df) {}

EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous) : m_isContinuous(continuous), m_Variant(r) {}

EnergyDistribution::EnergyDistribution() : EnergyDistribution(EnergyRange(100.0, 0.0), false) {}

float EnergyDistribution::selectEnergy() const {
    // the below code calls either DatFile::selectEnergy or
    // EnergyRange::selectEnergy depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> float { return arg.selectEnergy(m_isContinuous); };
    return std::visit(func, m_Variant);
}

float EnergyDistribution::getAverage() const {
    // the below code calls either DatFile::getAverage or
    // EnergyRange::getAverage depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> float { return arg.getAverage(); };
    return std::visit(func, m_Variant);
}

// EnergyRange impls

EnergyRange::EnergyRange(float centerEnergy, float energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}

float EnergyRange::selectEnergy(bool continuous) const {
    if (continuous) {
        return randomDoubleInRange(m_centerEnergy - m_energySpread / 2, m_centerEnergy + m_energySpread / 2);
    } else {
        float arr[3] = {m_centerEnergy - m_energySpread / 2, m_centerEnergy, m_centerEnergy + m_energySpread / 2};
        return arr[randomIntInRange(0, 2)];
    }
}

float EnergyRange::getAverage() const { return m_centerEnergy; }
}  // namespace RAYX
