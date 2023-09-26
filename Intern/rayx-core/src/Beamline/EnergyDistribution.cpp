#include "EnergyDistribution.h"
#include "LightSource.h"
#include "Random.h"

namespace RAYX {
EnergyDistribution::EnergyDistribution(DatFile df) :  m_Variant(df) {}

EnergyDistribution::EnergyDistribution(HardEdge he) : m_Variant(he) {}

EnergyDistribution::EnergyDistribution(SoftEdge se) : m_Variant(se) {}

EnergyDistribution::EnergyDistribution(SeperateEnergies sep) : m_Variant(sep) {}

EnergyDistribution::EnergyDistribution() : EnergyDistribution(HardEdge(100.0, 0.0)) {}

double EnergyDistribution::selectEnergy() const {
    // the below code calls either DatFile::selectEnergy,
    // HardEdge::selectEnergy, SoftEdge::selectEnergy or SeperateEnergies::selectEnergy
    // depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> double { return arg.selectEnergy(); };
    return std::visit(func, m_Variant);
}

double EnergyDistribution::getAverage() const {
    // the below code calls either DatFile::getAverage or
    // HardEdge::getAverage, SoftEdge::getAverage or SeperateEnergies::selectEnergy
    // depending on what is stored in m_Variant.
    const auto func = [&](const auto arg) -> double { return arg.getAverage(); };
    return std::visit(func, m_Variant);
}

//--------------------------------------------
// HardEdge impls

HardEdge::HardEdge(double centerEnergy, double energySpread) : m_centerEnergy(centerEnergy), m_energySpread(energySpread) {}

double HardEdge::selectEnergy() const {
    return randomDoubleInRange(m_centerEnergy - m_energySpread / 2, m_centerEnergy + m_energySpread / 2);
}

double HardEdge::getAverage() const { return m_centerEnergy; }


//SoftEdge impls

SoftEdge::SoftEdge(double centerEnergy, double sigma) : m_centerEnergy(centerEnergy), m_sigma(sigma) {}

double SoftEdge::selectEnergy() const {
    return randomNormal(m_centerEnergy, m_sigma);
}

double SoftEdge::getAverage() const { return m_centerEnergy; }


//seperateEnergies impls

SeperateEnergies::SeperateEnergies(double centerEnergy, double energySpread, int numOfEnergies) 
                : m_centerEnergy(centerEnergy), m_energySpread(energySpread), m_numberOfEnergies(numOfEnergies) {}


double SeperateEnergies::getAverage() const { return m_centerEnergy; }

double SeperateEnergies::selectEnergy() const { 
    
    double arr[m_numberOfEnergies];

    for (int i = 0; i < m_numberOfEnergies; i++) {
        arr[i] = (m_centerEnergy - m_energySpread / 2) + i * m_energySpread / (m_numberOfEnergies - 1);
    }

    return arr[randomIntInRange(0, m_numberOfEnergies-1)];


}
}  // namespace RAYX