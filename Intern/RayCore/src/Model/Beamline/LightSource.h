#pragma once
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "BeamlineObject.h"
#include "Core.h"
#include "EnergyDistribution.h"
#include "Tracer/Ray.h"
#include "glm.hpp"

namespace RAYX {
enum class SpreadType {  // default WhiteBand
    WhiteBand,
    ThreeEnergies
};
enum class EnergyDistributionType {
    File,
    Values,
    Total,
    Param
};  // default ET_VALUES
enum class SourceDistType {
    Simultaneous,
    HardEdge,
    Gauss
};  // default simultaneously

// TODO(rudi): unify!
enum class SourceDist { Uniform, Gaussian };

class RAYX_API LightSource : public BeamlineObject {
  public:
    LightSource(const char* name, EnergyDistribution dist, const double linPol0,
                const double linPol45, const double circPol,
                const std::array<double, 6> misalignment,
                const double sourceDepth, const double sourceHeight,
                const double sourceWidth, const double horDivergence,
                const double verDivergence);
    LightSource(const char* name, EnergyDistribution dist, const double linPol0,
                const double linPol45, const double circPol,
                const std::array<double, 6> misalignment);

    // Getter
    int getId();
    std::array<double, 6> getMisalignmentParams();
    double getLinear0();
    double getLinear45();
    double getCircular();
    double getVerDivergence() { return m_verDivergence; }
    double getHorDivergence() { return m_horDivergence; }
    double getSourceDepth() { return m_sourceDepth; }
    double getSourceHeight() { return m_sourceHeight; }
    double getSourceWidth() { return m_sourceWidth; }

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    double getPhotonEnergy() const;

    double selectEnergy();
    glm::dvec3 getDirectionFromAngles(double phi, double psi);
    // get the rays according to specific light source, has to be implemented in
    // each class that inherits from LightSource
    virtual std::vector<Ray> getRays() = 0;

    LightSource();
    virtual ~LightSource();

    /** the energy distribution used when deciding the energies of the rays. */
    const EnergyDistribution m_EnergyDistribution;

  protected:
    // Geometric Parameters
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // in rad:
    double m_horDivergence;
    double m_verDivergence;

    std::uniform_real_distribution<double> m_uniformDist;
    std::normal_distribution<double> m_normDist;
    std::default_random_engine m_randEngine;

  private:
    // User/Design Parameter
    std::array<double, 6> m_misalignmentParams;  // x, y, psi, phi

    // Physics Parameters
    // point source & matrix source
    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    // TODO(Jannis): move to children
    SourceDistType m_sourceDistributionType;
};

}  // namespace RAYX
