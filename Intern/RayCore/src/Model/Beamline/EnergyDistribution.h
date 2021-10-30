#pragma once

#include <variant>
#include <random>

#include <Data/dat.h>

namespace RAYX {
    /** describes the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy + m_EnergySpread/2]` */
    struct EnergyRange {
        double m_CenterEnergy;
        double m_EnergySpread;

        EnergyRange(double centerEnergy, double EnergySpread);

        double selectEnergy(std::mt19937& rng, bool continuous) const;
    };

    class EnergyDistribution {
        public:
            EnergyDistribution(); // TODO this default-constructor is required because LightSource also has one, do we actually want it though?
            EnergyDistribution(DatFile, bool continuous);
            EnergyDistribution(EnergyRange, bool continuous);

            double selectEnergy();

        private:
            /** Shows whether the distribution is continuous or discrete */
            bool m_IsContinuous;
            static std::mt19937 rng;

            std::variant<DatFile, EnergyRange> m_Variant;
    };
}