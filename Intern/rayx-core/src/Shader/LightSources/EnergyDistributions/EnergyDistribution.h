#pragma once

#include "Core.h"

namespace RAYX {

struct EnergyDistributionList {
    double* __restrict data;
    int size;
};

using EnergyDistributionDataVariant = std::variant<std::monostate, HardEdge, SoftEdge, SeparateEnergies, EnergyDistributionList>;

}  // namespace RAYX
