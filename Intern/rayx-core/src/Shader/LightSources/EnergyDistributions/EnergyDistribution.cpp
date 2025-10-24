#include "EnergyDistribution.h"

namespace {

RAYX_FN_ACC
int binarySearchPrefix(const double* __restrict prefixWeights, int size, double r) {
    int left  = 0;
    int right = size - 1;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (r > prefixWeights[mid]) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

}  // unnamed namespace

namespace rayx {

RAYX_FN_ACC double selectEnergy(const HardEdge& __restrict hardEdge, Rand& __restrict rand) {
    const auto a   = hardEdge.m_centerEnergy - hardEdge.m_energySpread / 2.0;
    const auto b   = hardEdge.m_centerEnergy + hardEdge.m_energySpread / 2.0;
    const auto min = std::min(a, b);
    const auto max = std::max(a, b);
    return rand.randomDoubleInRange(min, max);
}

RAYX_FN_ACC double selectEnergy(const SoftEdge& __restrict softEdge, Rand& __restrict rand) {
    return rand.randomDoubleNormalDistributed(softEdge.m_centerEnergy, softEdge.m_sigma);
}

RAYX_FN_ACC double selectEnergy(const SeparateEnergies& __restrict separateEnergies, Rand& __restrict rand) {
    // separateEnergies.m_numberOfEnergies is expected to be equal or greater to 1

    const auto n =
        // random number between 0 and number of energies
        rand.randomIntInRange(0, separateEnergies.m_numberOfEnergies)
            // normalize number between 0 and 1. use std::max to compensate for the case that number of energies is 1
            / std::max(1.0, separateEnergies.m_numberOfEnergies - 1.0)
        // shift by -0.5 so that the range is between -0.5 and +0.5. use std::min to not shift in case number of energies is 1
        - std::min(0.5, separateEnergies.m_numberOfEnergies - 1.0);

    return separateEnergies.m_centerEnergy + n * separateEnergies.m_energySpread;
}

RAYX_FN_ACC double selectEnergy(const EnergyDistributionList& __restrict energyDistributionList, Rand& __restrict rand) {
    // TODO: implement all the other stuff from DayFile::selectEnergy
    // TODO: implement continous
    const auto r    = rand.randomDouble() * energyDistributionList.weightSum;
    const int index = binarySearchPrefix(energyDistributionList.prefixWeights, energyDistributionList.size, r);
    return energyDistributionList.energies[index];
}

RAYX_FN_ACC double selectEnergy(const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand) {
    return std::visit([&]<typename T>(const T& __restrict value) { return selectEnergy(value, rand); }, energyDistribution);
}

}  // namespace rayx
