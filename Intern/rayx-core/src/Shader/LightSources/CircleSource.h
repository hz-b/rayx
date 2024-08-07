#pragma once

#include "Shader/LightSource.h"

namespace RAYX {

struct CircleSource : LightSource {
    CircleSource() = default;
    inline CircleSource(const DesignSource& designSource);

    RAYX_FN_ACC
    glm::dvec3 getDirection(Rand& rand) const;

    RAYX_FN_ACC
    Ray getRay(int lightSourceId, Rand& rand) const;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_energyDistribution;

    ElectricField m_field = {{}, {}, {}};

    int m_numOfCircles;
    Rad m_maxOpeningAngle;
    Rad m_minOpeningAngle;
    Rad m_deltaOpeningAngle;
};

CircleSource::CircleSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_energyDistribution(designSource.getEnergyDistribution()),
      m_field(stokesToElectricField(designSource.getStokes())),
      m_numOfCircles(designSource.getNumOfCircles()),
      m_maxOpeningAngle(designSource.getMaxOpeningAngle()),
      m_minOpeningAngle(designSource.getMinOpeningAngle()),
      m_deltaOpeningAngle(designSource.getDeltaOpeningAngle()) {}

}  // namespace RAYX
