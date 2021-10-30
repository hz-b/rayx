#include <iostream>
#include <utility>

#include "EnergyDistribution.h"

namespace RAYX {
    EnergyDistribution::EnergyDistribution(DatFile df, bool continuous)
        : m_IsContinuous(continuous), m_Variant(df) {}

    EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous)
        : m_IsContinuous(continuous), m_Variant(r) {}

    EnergyDistribution::EnergyDistribution()
        : EnergyDistribution(EnergyRange(100.0, 0.0), false) {}

    double EnergyDistribution::selectEnergy() {
        const auto func = [&](const auto arg) -> double { return arg.selectEnergy(m_IsContinuous); };
        return std::visit(func, m_Variant);
    }


    EnergyRange::EnergyRange(double centerEnergy, double EnergySpread)
        : m_CenterEnergy(centerEnergy), m_EnergySpread(EnergySpread) {}

    double EnergyRange::selectEnergy(bool continuous) const {
        return 0; // TODO(rudi)
    }

}