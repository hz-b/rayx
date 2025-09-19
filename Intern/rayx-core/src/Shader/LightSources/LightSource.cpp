#include "LightSource.h"

#include <cmath>

#include "Beamline/EnergyDistribution.h"
#include "Design/DesignSource.h"

namespace RAYX {
LightSourceBase::LightSourceBase(const DesignSource& dSource)
    : m_numberOfRays(static_cast<uint32_t>(dSource.getNumberOfRays())),
      m_orientation(dSource.getOrientation()),
      m_position(dSource.getPosition())
      {}

// needed for many of the light sources, from two angles to one direction vector
RAYX_FN_ACC
glm::dvec3 LightSourceBase::getDirectionFromAngles(const double phi, const double psi) {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return {al, am, an};
}

//  (see RAYX.FOR select_energy)
//  TODO: enable
// double LightSourceBase::selectEnergy() const { return m_EnergyDistribution.selectEnergy(); }
// double LightSourceBase::selectEnergy() const { return 300.0; }

}  // namespace RAYX
