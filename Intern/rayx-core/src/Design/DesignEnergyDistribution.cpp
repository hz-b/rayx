#include "DesignEnergyDistribution.h"

namespace RAYX {

double DesignEnergyDistribution::selectEnergy() const {
    return visit([](const auto& x) {
        return x.selectEnergy();
    });
}

double DesignHardEdge::selectEnergy() const {
    double center = field<CenterEnergyStr>();
    double spread = field<EnergySpreadStr>();
    return randomDoubleInRange(center - spread / 2, center + spread / 2);
}

double DesignSoftEdge::selectEnergy() const {
    double center = field<CenterEnergyStr>();
    double sigma = field<SigmaStr>();
    return randomNormal(center, sigma);
}

double DesignSeparateEnergies::selectEnergy() const {
    double center = field<CenterEnergyStr>();
    double spread = field<EnergySpreadStr>();
    int numberOfEnergies = field<NumberOfEnergiesStr>();

    // choose random spike from range of seperate energies
    // from energyspread calculate energy for given spike
    if (numberOfEnergies == 1) {
        return center;
    }

    int randomenergy = randomIntInRange(0, numberOfEnergies - 1);
    double energy = (center - spread / 2) + randomenergy * spread / (numberOfEnergies - 1);

    return energy;
}

}
