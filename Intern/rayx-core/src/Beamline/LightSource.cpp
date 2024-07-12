#include "LightSource.h"

#include <cmath>

#include "DesignElement/DesignSource.h"
#include "Beamline/EnergyDistribution.h"

namespace RAYX {
LightSource::LightSource(const DesignSource& dSource)
    : m_name(dSource.getName()),
      m_EnergyDistribution(dSource.getEnergyDistribution()),
      m_numberOfRays(static_cast<uint32_t>(dSource.getNumberOfRays())),
      m_verDivergence(0.0),
      m_orientation(dSource.getWorldOrientation()),
      m_position(dSource.getWorldPosition()),
      m_misalignmentParams(dSource.getMisalignment()) {}

Misalignment LightSource::getMisalignmentParams() const { return m_misalignmentParams; }

// needed for many of the light sources, from two angles to one direction vector
glm::dvec3 LightSource::getDirectionFromAngles(const double phi, const double psi) {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return {al, am, an};
}

//  (see RAYX.FOR select_energy)
double LightSource::selectEnergy() const { return m_EnergyDistribution.selectEnergy(); }

double LightSource::calcPhotonWavelength(double photonEnergy) {
    // Energy Distribution Type : Values only
    double photonWaveLength = photonEnergy == 0.0 ? 0 : inm2eV / photonEnergy;  // i nm to eV
    return photonWaveLength;
}

}  // namespace RAYX
