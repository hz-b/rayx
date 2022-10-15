#include "LightSource.h"

#include <cmath>
#include <utility>

namespace RAYX {
LightSource::LightSource(const DesignObject& dobj) {
    m_name = dobj.name();
    m_EnergyDistribution = dobj.parseEnergyDistribution();
    m_numberOfRays = dobj.parseNumberRays();
    m_sourceDepth = dobj.parseSourceDepth();
    m_sourceHeight = dobj.parseSourceHeight();
    m_sourceWidth = dobj.parseSourceWidth();
    m_horDivergence = dobj.parseHorDiv();
    m_verDivergence = dobj.parseVerDiv();
    m_misalignmentParams = dobj.parseMisalignment();
    m_linearPol_0 = dobj.parseLinearPol0();
    m_linearPol_45 = dobj.parseLinearPol45();
    m_circularPol = dobj.parseCircularPol();
}

double LightSource::getLinear0() const { return m_linearPol_0; }

double LightSource::getLinear45() const { return m_linearPol_45; }

double LightSource::getCircular() const { return m_circularPol; }

std::array<double, 6> LightSource::getMisalignmentParams() const { return m_misalignmentParams; }

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
