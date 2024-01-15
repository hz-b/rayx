#include "EnergyDistribution.h"

#include "LightSource.h"
#include "Random.h"

namespace RAYX {
EnergyDistribution::EnergyDistribution(DatFile df) : m_Variant(df) {}

EnergyDistribution::EnergyDistribution(HardEdge he) : m_Variant(he) {}

EnergyDistribution::EnergyDistribution(SoftEdge se) : m_Variant(se) {}

EnergyDistribution::EnergyDistribution(SeperateEnergies sep) : m_Variant(sep) {}

EnergyDistribution::EnergyDistribution() : EnergyDistribution(HardEdge(100.0, 0.0)) {}

double EnergyDistribution::selectEnergy() const {
    // the below code calls either DatFile::selectEnergy,
    // HardEdge::selectEnergy, SoftEdge::selectEnergy or SeperateEnergies::selectEnergy
    // depending on what is stored in m_Variant.

    // If you want to know more about this "lambda-expression" syntax,
    // you can read it up at https://en.cppreference.com/w/cpp/language/lambda
    const auto func = [&](const auto arg) -> double { return arg.selectEnergy(); };
    return std::visit(func, m_Variant);
}

//--------------------------------------------
// HardEdge impls

HardEdge::HardEdge(double centerEnergy, double energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}

double HardEdge::selectEnergy() const { return randomDoubleInRange(m_centerEnergy - m_energySpread / 2, m_centerEnergy + m_energySpread / 2); }

// SoftEdge impls

SoftEdge::SoftEdge(double centerEnergy, double sigma) : m_centerEnergy(centerEnergy), m_sigma(sigma) {}

double SoftEdge::selectEnergy() const { return randomNormal(m_centerEnergy, m_sigma); }

// seperateEnergies impls

SeperateEnergies::SeperateEnergies(double centerEnergy, double energySpread, int numOfEnergies)
    : m_centerEnergy(centerEnergy), m_energySpread(energySpread), m_numberOfEnergies(numOfEnergies) {}

double SeperateEnergies::selectEnergy() const {
    // choose random spike from range of seperate energies
    // from energyspread calculate energy for given spike
    if (m_numberOfEnergies == 1) {
        return m_centerEnergy;
    }

    int randomenergy = randomIntInRange(0, m_numberOfEnergies - 1);
    double energy = (m_centerEnergy - m_energySpread / 2) + randomenergy * m_energySpread / (m_numberOfEnergies - 1);

    return energy;
}
}  // namespace RAYX
