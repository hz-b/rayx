#include "LightSource.h"

#include <cmath>

namespace RAYX {
LightSource::LightSource(const DesignObject& dobj) {
    m_name = dobj.name();
    m_EnergyDistribution = dobj.parseEnergyDistribution();
    m_misalignmentParams = dobj.parseMisalignment();
    m_numberOfRays = dobj.parseNumberRays();
    m_sourceHeight = dobj.parseSourceHeight();
    m_sourceWidth = dobj.parseSourceWidth();
    m_horDivergence = dobj.parseHorDiv();
    m_orientation = dobj.parseOrientation();
    m_position = dobj.parsePosition();
}

Misalignment LightSource::getMisalignmentParams() const { return m_misalignmentParams; }

[[maybe_unused]] float LightSource::getPhotonEnergy() const { return m_EnergyDistribution.getAverage(); }

// needed for many of the light sources, from two angles to one direction vector
glm::vec3 LightSource::getDirectionFromAngles(const float phi, const float psi) const {
    float al = cos(psi) * sin(phi);
    float am = -sin(psi);
    float an = cos(psi) * cos(phi);
    return {al, am, an};
}

//  (see RAYX.FOR select_energy)
float LightSource::selectEnergy() const { return m_EnergyDistribution.selectEnergy(); }

}  // namespace RAYX
