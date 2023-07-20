#include "LightSource.h"

#include <cmath>

namespace RAYX {
LightSource::LightSource(const DesignObject& dobj) {
    m_name = dobj.name();
    m_EnergyDistribution = dobj.parseEnergyDistribution();
    m_numberOfRays = dobj.parseNumberRays();
    m_sourceHeight = dobj.parseSourceHeight();
    m_sourceWidth = dobj.parseSourceWidth();
    m_horDivergence = dobj.parseHorDiv();
    m_orientation = dobj.parseOrientation();
    m_position = dobj.parsePosition();
}

[[maybe_unused]] double LightSource::getPhotonEnergy() const { return m_EnergyDistribution.getAverage(); }

// needed for many of the light sources, from two angles to one direction vector
glm::dvec3 LightSource::getDirectionFromAngles(const double phi, const double psi) const {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return {al, am, an};
}

//  (see RAYX.FOR select_energy)
double LightSource::selectEnergy() const { return m_EnergyDistribution.selectEnergy(); }

}  // namespace RAYX
