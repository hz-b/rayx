#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct PixelSource : LightSource {
    PixelSource() = default;
    inline PixelSource(const DesignSource& designSource);

    RAYX_FN_ACC
    double getPosInDistribution(SourceDist l, double extent, Rand& rand) const;

    RAYX_FN_ACC
    Ray getRay(int32_t lightSourceId, Rand& rand) const;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_energyDistribution;

    ElectricField m_field = {{}, {}, {}};
};

PixelSource::PixelSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_energyDistribution(designSource.getEnergyDistribution()),
      m_field(stokesToElectricField(designSource.getStokes())) {}

}  // namespace RAYX
