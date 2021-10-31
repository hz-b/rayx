#include <iostream>
#include <utility>
#include <ctime>

#include "EnergyDistribution.h"

namespace RAYX {
    std::mt19937 EnergyDistribution::rng(time(NULL));

    EnergyDistribution::EnergyDistribution(DatFile df, bool continuous)
        : m_IsContinuous(continuous), m_Variant(df) {}

    EnergyDistribution::EnergyDistribution(EnergyRange r, bool continuous)
        : m_IsContinuous(continuous), m_Variant(r) {}

    EnergyDistribution::EnergyDistribution()
        : EnergyDistribution(EnergyRange(100.0, 0.0), false) {}

    double EnergyDistribution::selectEnergy() const {
        // the below code calls either DatFile::selectEnergy or EnergyRange::selectEnergy depending on what is stored in m_Variant.
        const auto func = [&](const auto arg) -> double { return arg.selectEnergy(rng, m_IsContinuous); };
        return std::visit(func, m_Variant);
    }

    double EnergyDistribution::getAverage() const {
        // the below code calls either DatFile::getAverage or EnergyRange::getAverage depending on what is stored in m_Variant.
        const auto func = [&](const auto arg) -> double { return arg.getAverage(); };
        return std::visit(func, m_Variant);
    }

    // EnergyRange impls

    EnergyRange::EnergyRange(double centerEnergy, double EnergySpread)
        : m_CenterEnergy(centerEnergy), m_EnergySpread(EnergySpread) {}

    double EnergyRange::selectEnergy(std::mt19937& rng, bool continuous) const {
        if (continuous) {
            double percentage = ((double) rng()) / std::mt19937::max(); // in [0, 1]
            return m_CenterEnergy + ((percentage - 0.5) * m_EnergySpread);
        } else {
            double arr[3] = {m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy, m_CenterEnergy + m_EnergySpread/2};
            return arr[rng()%3]; // this chooses a random of the 3 elements of `arr` above
        }
    }

    double EnergyRange::getAverage() const {
        return m_CenterEnergy;
    }
}