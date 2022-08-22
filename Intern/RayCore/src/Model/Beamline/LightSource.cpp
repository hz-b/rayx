#include "LightSource.h"

#include <cmath>
#include <utility>

namespace RAYX {
LightSource::LightSource(const char* name, EnergyDistribution dist,
                         const double linPol0, const double linPol45,
                         const double circPol,
                         const std::array<double, 6> misalignment,
                         const double sourceDepth, const double sourceHeight,
                         const double sourceWidth, const double horDivergence,
                         const double verDivergence)
    : m_name(name),
      m_EnergyDistribution(std::move(dist)),
      m_sourceDepth(sourceDepth),
      m_sourceHeight(sourceHeight),
      m_sourceWidth(sourceWidth),
      m_horDivergence(horDivergence),
      m_verDivergence(verDivergence),
      m_misalignmentParams(misalignment),
      m_linearPol_0(linPol0),
      m_linearPol_45(linPol45),
      m_circularPol(circPol) {}

// ! Temporary code clone
LightSource::LightSource(const char* name, EnergyDistribution dist,
                         const double linPol0, const double linPol45,
                         const double circPol,
                         const std::array<double, 6> misalignment)
    : m_name(name),
      m_EnergyDistribution(std::move(dist)),
      m_misalignmentParams(misalignment),
      m_linearPol_0(linPol0),
      m_linearPol_45(linPol45),
      m_circularPol(circPol) {}

double LightSource::getLinear0() const { return m_linearPol_0; }

double LightSource::getLinear45() const { return m_linearPol_45; }

double LightSource::getCircular() const { return m_circularPol; }

std::array<double, 6> LightSource::getMisalignmentParams() const {
    return m_misalignmentParams;
}

[[maybe_unused]] double LightSource::getPhotonEnergy() const {
    return m_EnergyDistribution.getAverage();
}

// needed for many of the light sources, from two angles to one direction vector
glm::dvec3 LightSource::getDirectionFromAngles(const double phi,
                                               const double psi) const {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return {al, am, an};
}

//  (see RAYX.FOR select_energy)
double LightSource::selectEnergy() const {
    return m_EnergyDistribution.selectEnergy();
}
LightSource::LightSource() = default;

LightSource::~LightSource() = default;

}  // namespace RAYX
