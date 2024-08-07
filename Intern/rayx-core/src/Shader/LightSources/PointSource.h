#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct PointSource : LightSource {
    PointSource() = default;
    inline PointSource(const DesignSource& designSource);

    RAYX_FN_ACC
    Ray getRay(int lightSourceId, Rand& rand) const;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_energyDistribution;

    ElectricField m_field = {{}, {}, {}};

    // Geometric Params
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
};

PointSource::PointSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_energyDistribution(designSource.getEnergyDistribution()),
      m_field(stokesToElectricField(designSource.getStokes())),
      m_widthDist(designSource.getWidthDist()),
      m_heightDist(designSource.getHeightDist()),
      m_horDist(designSource.getHorDist()),
      m_verDist(designSource.getVerDist()) {}

}  // namespace RAYX
