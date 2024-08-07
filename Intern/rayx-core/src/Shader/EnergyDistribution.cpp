#include "EnergyDistribution.h"

#include "LightSource.h"
#include "Random.h"

namespace RAYX {

RAYX_FN_ACC
double HardEdge::selectEnergy(Rand& rand) const {
    return rand.randomDoubleInRange(m_centerEnergy - m_energySpread / 2, m_centerEnergy + m_energySpread / 2);
}

RAYX_FN_ACC
// TODO(Sven): is Rand::randomDoubleNormalDistributed the same as randomNormal from Random.cpp ?
double SoftEdge::selectEnergy(Rand& rand) const { return rand.randomDoubleNormalDistributed(m_centerEnergy, m_sigma); }

RAYX_FN_ACC
double SeparateEnergies::selectEnergy(Rand& rand) const {
    // choose random spike from range of separate energies
    // from energyspread calculate energy for given spike
    if (m_numberOfEnergies == 1) {
        return m_centerEnergy;
    }

    int randomenergy = rand.randomIntInRange(0, m_numberOfEnergies - 1);
    double energy = (m_centerEnergy - m_energySpread / 2) + randomenergy * m_energySpread / (m_numberOfEnergies - 1);

    return energy;
}

RAYX_FN_ACC
double EnergyDistribution::selectEnergy(Rand& rand) const {
    // the below code calls either SampleEnergyDistribution::selectEnergy,
    // HardEdge::selectEnergy, SoftEdge::selectEnergy or SeparateEnergies::selectEnergy
    // depending on what is stored in m_variant.

    const auto func = [&](const auto arg) -> double { return arg.selectEnergy(rand); };
    return std::visit(func, m_variant);
}

}  // namespace RAYX
