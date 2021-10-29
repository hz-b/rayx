#include <iostream>
#include <utility>

#include "EnergyDistribution.h"

namespace RAYX {
    EnergyDistribution::EnergyDistribution(DatFile df, bool continuous)
        : m_IsContinuous(continuous), m_EnergyDistributionType(ET_FILE), m_Variant(df) {}

    EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous)
        : m_IsContinuous(continuous), m_EnergyDistributionType(ET_VALUES), m_Variant(r) {}

    double EnergyDistribution::selectEnergy() {
        const auto func = [&](const auto arg) -> double { return arg.selectEnergy(m_IsContinuous); };
        return std::visit(func, m_Variant);
    }

    double EnergyRange::selectEnergy(bool continuous) const {
        return 0; // TODO(rudi)
    }

}