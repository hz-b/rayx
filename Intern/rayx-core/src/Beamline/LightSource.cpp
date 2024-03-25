#include "LightSource.h"

#include <cmath>

#include "EnergyDistribution.h"

namespace RAYX {
LightSource::LightSource(const DesignSource& deso) {
    m_name = deso.getName();
    m_EnergyDistribution = deso.getEnergyDistribution();
    m_misalignmentParams = deso.getMisalignment();
    m_numberOfRays = (uint32_t) deso.getNumberOfRays();
    m_orientation = deso.getWorldOrientation();
    m_position = deso.getWorldPosition();
    m_verDivergence = 0.0;
}

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
