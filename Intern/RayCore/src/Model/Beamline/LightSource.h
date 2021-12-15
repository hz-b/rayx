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
#include "Presenter/SimulationEnv.h"
#include "Tracer/Ray.h"
#include "glm.hpp"

namespace RAYX {
enum SPREAD_TYPE { ES_WHITE_BAND, ES_THREE_ENERGIES };  // default ES_WHITE_BAND
enum ENERGY_DISTRIBUTION_TYPE {
    ET_FILE,
    ET_VALUES,
    ET_TOTAL,
    ET_PARAM
};  // default ET_VALUES
enum SOURCE_DISTRIBUTION_TYPE {
    ST_SIMULTANEOUS,
    ST_HARD_EDGE,
    ST_GAUSS
};  // default simultaneously

class RAYX_API LightSource : public BeamlineObject {
  public:
    LightSource(const char* name, EnergyDistribution dist, const double linPol0,
                const double linPol45, const double circPol,
                const std::vector<double> misalignment,
                const double sourceDepth, const double sourceHeight,
                const double sourceWidth, const double horDivergence,
                const double verDivergence);
    LightSource(const char* name, EnergyDistribution dist, const double linPol0,
                const double linPol45, const double circPol,
                const std::vector<double> misalignment);

    // Getter
    int getId();
    std::vector<double> getMisalignmentParams();
    double getLinear0();
    double getLinear45();
    double getCircular();

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    double getPhotonEnergy() const;

    double selectEnergy();
    void setNumberOfRays(int numberOfRays);
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
    double m_horDivergence;
    double m_verDivergence;

    std::uniform_real_distribution<double> m_uniformDist;
    std::normal_distribution<double> m_normDist;
    std::default_random_engine m_randEngine;

  private:
    // User/Design Parameter
    std::vector<double> m_misalignmentParams;  // x, y, psi, phi

    // Physics Parameters
    // point source & matrix source
    double m_linearPol_0;
    double m_linearPol_45;
    double m_circularPol;

    // TODO(Jannis): move to children
    SOURCE_DISTRIBUTION_TYPE m_sourceDistributionType;
};

}  // namespace RAYX