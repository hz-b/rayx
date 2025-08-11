#include "EnergyDistribution.h"

#include "Random.h"

namespace RAYX {
//--------------------------------------------
// HardEdge impls

HardEdge::HardEdge(double centerEnergy, double energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}

// SoftEdge impls

SoftEdge::SoftEdge(double centerEnergy, double sigma) : m_centerEnergy(centerEnergy), m_sigma(sigma) {}

// separateEnergies impls

SeparateEnergies::SeparateEnergies(double centerEnergy, double energySpread, int numOfEnergies)
    : m_centerEnergy(centerEnergy), m_energySpread(energySpread), m_numberOfEnergies(numOfEnergies) {}

}  // namespace RAYX
