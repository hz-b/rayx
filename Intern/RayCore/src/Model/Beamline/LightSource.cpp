#include "LightSource.h"

#include <cmath>

namespace RAYX {
LightSource::LightSource(const char* name, EnergyDistribution dist,
                         const double linPol0, const double linPol45,
                         const double circPol,
                         const std::array<double, 6> misalignment,
                         const double sourceDepth, const double sourceHeight,
                         const double sourceWidth, const double horDivergence,
                         const double verDivergence)
    : BeamlineObject(name),
      m_EnergyDistribution(dist),
      m_sourceDepth(sourceDepth),
      m_sourceHeight(sourceHeight),
      m_sourceWidth(sourceWidth),
      m_horDivergence(horDivergence),
      m_verDivergence(verDivergence),
      m_misalignmentParams(misalignment),
      m_linearPol_0(linPol0),
      m_linearPol_45(linPol45),
      m_circularPol(circPol) {
    std::normal_distribution<double> m_normDist(0, 1);
    std::uniform_real_distribution<double> m_uniformDist(0, 1);
    std::default_random_engine m_randEngine;
}

// ! Temporary code clone
LightSource::LightSource(const char* name, EnergyDistribution dist,
                         const double linPol0, const double linPol45,
                         const double circPol,
                         const std::array<double, 6> misalignment)
    : BeamlineObject(name),
      m_EnergyDistribution(dist),
      m_misalignmentParams(misalignment),
      m_linearPol_0(linPol0),
      m_linearPol_45(linPol45),
      m_circularPol(circPol) {}

double LightSource::getLinear0() { return m_linearPol_0; }

double LightSource::getLinear45() { return m_linearPol_45; }

double LightSource::getCircular() { return m_circularPol; }

std::array<double, 6> LightSource::getMisalignmentParams() {
    return m_misalignmentParams;
}

double LightSource::getPhotonEnergy() const {
    return m_EnergyDistribution.getAverage();
}

// needed for many of the light sources, from two angles to one direction vector
glm::dvec3 LightSource::getDirectionFromAngles(const double phi,
                                               const double psi) {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return glm::dvec3(al, am, an);
}

//  (see RAYX.FOR select_energy)
double LightSource::selectEnergy() {
    return m_EnergyDistribution.selectEnergy();
}

LightSource::~LightSource() {}

}  // namespace RAYX
