#pragma once

#include "Beamline/EnergyDistribution.h"
#include "Core.h"
#include "Shader/Rand.h"

namespace rayx {

struct EnergyDistributionList {
    double* __restrict prefixWeights;
    double* __restrict energies;
    double weightSum;
    int size;
    bool continous;
};

// TODO: use cuda::std::variant
using EnergyDistributionDataVariant = std::variant<HardEdge, SoftEdge, SeparateEnergies, EnergyDistributionList>;

RAYX_FN_ACC double selectEnergy(const HardEdge& __restrict hardEdge, Rand& __restrict rand);
RAYX_FN_ACC double selectEnergy(const SoftEdge& __restrict softEdge, Rand& __restrict rand);
RAYX_FN_ACC double selectEnergy(const SeparateEnergies& __restrict separateEnergies, Rand& __restrict rand);
RAYX_FN_ACC double selectEnergy(const EnergyDistributionList& __restrict energyDistributionList, Rand& __restrict rand);
RAYX_FN_ACC double selectEnergy(const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand);

}  // namespace rayx
