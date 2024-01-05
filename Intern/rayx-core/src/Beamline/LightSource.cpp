#include "LightSource.h"

#include <cmath>

#include "EnergyDistribution.h"

namespace RAYX {
LightSource::LightSource(const DesignObject& dobj) {
    m_name = dobj.name();
    m_EnergyDistribution = dobj.parseEnergyDistribution();
    m_misalignmentParams = dobj.parseMisalignment();
    m_numberOfRays = dobj.parseNumberRays();
    m_orientation = dobj.parseOrientation();
    m_position = dobj.parsePosition();
    m_verDivergence = 0.0;
}

Misalignment LightSource::getMisalignmentParams() const { return m_misalignmentParams; }

[[maybe_unused]] double LightSource::getPhotonEnergy() const { return m_EnergyDistribution.getAverage(); }

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
    double photonWaveLength = photonEnergy == 0.0 ? 0 : inm2eV / photonEnergy; //i nm to eV
    return photonWaveLength;
}

}  // namespace RAYX
