#pragma once

#include <variant>
#include <random>

#include <Data/DatFile.h>

namespace RAYX {
    /** describes the interval `[m_CenterEnergy - m_EnergySpread/2, m_CenterEnergy + m_EnergySpread/2]` */
    struct EnergyRange {
        double m_CenterEnergy;
        double m_EnergySpread;

        EnergyRange(double centerEnergy, double EnergySpread);

        double selectEnergy(std::mt19937& rng, bool continuous) const;
        double getAverage() const;
    };

    /**
     * The class EnergyDistribution is contained in LightSources to describe the mathematical distribution from which the energy of the rays are sampled.
     * It can either be a `EnergyRange` being a uniform distribution in some interval, or a `DatFile` which means that the distribution is loaded from a .DAT file.
    */
    class EnergyDistribution {
        public:
            EnergyDistribution(); // TODO this default-constructor is required because LightSource also has one, do we actually want it though?
            EnergyDistribution(DatFile, bool continuous);
            EnergyDistribution(EnergyRange, bool continuous);

            /** The selectEnergy() function returns one sample from the underlying distribution */
            double selectEnergy() const;
            /** yields the expected value of the distribution */
            double getAverage() const;

        private:
            /** Shows whether the distribution is continuous or discrete */
            bool m_IsContinuous;

            /** the device for randomness, call `rng()` to get a random number */
            static std::mt19937 rng;

            /** stores either a DatFile or an EnergyRange, depending on the constructor used to create this */
            std::variant<DatFile, EnergyRange> m_Variant;
    };
}