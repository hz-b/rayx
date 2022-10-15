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

LightSource::LightSource(const char* name, uint32_t numberOfRays, EnergyDistribution dist, const double linPol0, const double linPol45,
                         const double circPol, const std::array<double, 6> misalignment, const double sourceDepth,
                         const double sourceHeight, const double sourceWidth, const double horDivergence, const double verDivergence)
    : m_name(name),
      m_EnergyDistribution(std::move(dist)),
      m_numberOfRays(numberOfRays),
      m_sourceDepth(sourceDepth),
      m_sourceHeight(sourceHeight),
      m_sourceWidth(sourceWidth),
      m_horDivergence(horDivergence),
      m_verDivergence(verDivergence),
      m_misalignmentParams(misalignment),
      m_linearPol_0(linPol0),
      m_linearPol_45(linPol45),
      m_circularPol(circPol) {}

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
