#pragma once

#include <variant>

#include <Data/dat.h>

namespace RAYX {
    //TODO: ET_TOTAL and ET_PARAM are unused
    enum ENERGY_DISTRIBUTION_TYPE { ET_FILE, ET_VALUES, ET_TOTAL, ET_PARAM }; // default ET_VALUES

    /** describes the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy + m_EnergySpread/2]` */
    struct EnergyRange {
        double m_CenterEnergy;
        double m_EnergySpread;

        double selectEnergy(bool continuous) const;
    };

    class EnergyDistribution {
        public:
            EnergyDistribution(DatFile, bool continuous);
            EnergyDistribution(EnergyRange, bool continuous);

            double selectEnergy();

        private:
            /** Shows whether the distribution is continuous or discrete */
            bool m_IsContinuous;

            ENERGY_DISTRIBUTION_TYPE m_EnergyDistributionType;
            std::variant<DatFile, EnergyRange> m_Variant;
    };
}