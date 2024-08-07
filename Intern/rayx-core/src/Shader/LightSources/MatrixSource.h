#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct MatrixSource : LightSource {
    MatrixSource() = default;
    inline MatrixSource(const DesignSource& designSource);

    RAYX_FN_ACC
    Ray getRay(int rayId, int lightSourceId, Rand& rand) const;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_energyDistribution;

    ElectricField m_field = {{}, {}, {}};
};

MatrixSource::MatrixSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_energyDistribution(designSource.getEnergyDistribution()),
      m_field(stokesToElectricField(designSource.getStokes())) {}

}  // namespace RAYX
