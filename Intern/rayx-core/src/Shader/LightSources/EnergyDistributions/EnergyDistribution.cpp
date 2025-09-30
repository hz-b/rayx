#include "EnergyDistribution.h"

namespace {

RAYX_FN_ACC
int binarySearchPrefix(const double* __restrict prefix, int n, double r) {
    int left  = 0;
    int right = n - 1;
    while (left < right) {
        const int mid = (left + right) >> 1;  // divide by 2
        if (prefix[mid] > r)
            right = mid;
        else
            left = mid + 1;
    }
    return left;
}

}  // unnamed namespace

namespace RAYX {

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
    // choose random spike from range of separate energies
    // from energyspread calculate energy for given spike
    if (separateEnergies.m_numberOfEnergies == 1) { return separateEnergies.m_centerEnergy; }

    int randomenergy = rand.randomIntInRange(0, separateEnergies.m_numberOfEnergies - 1);
    double energy    = (separateEnergies.m_centerEnergy - separateEnergies.m_energySpread / 2) +
                    randomenergy * separateEnergies.m_energySpread / (separateEnergies.m_numberOfEnergies - 1);

    return energy;
}

RAYX_FN_ACC double selectEnergy(const EnergyDistributionList& __restrict energyDistributionList, Rand& __restrict rand) {
    // TODO: implement all the other stuff from DayFile::selectEnergy
    const auto r = rand.randomDouble() * energyDistributionList.weightSum;
    // TODO: test this
    const int index = binarySearchPrefix(energyDistributionList.prefixWeights, energyDistributionList.size, r);
    return energyDistributionList.energies[index];
}

RAYX_FN_ACC double selectEnergy(const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand) {
    return std::visit([&]<typename T>(const T& __restrict value) { return selectEnergy(value, rand); }, energyDistribution);
}

}  // namespace RAYX
