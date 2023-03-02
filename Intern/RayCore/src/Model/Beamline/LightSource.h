#pragma once
#include <array>
#include <glm.hpp>
#include <string>
#include <vector>

#include "Core.h"
#include "Data/xml.h"
#include "EnergyDistribution.h"
#include "Tracer/Ray.h"

namespace RAYX {
enum class SpreadType { WhiteBand, ThreeEnergies };                // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };  // default ET_VALUES
enum class SourceDistType { Simultaneous, HardEdge, Gauss };       // default simultaneously
enum class SourceDist { Uniform, Gaussian };                       // TODO(rudi): unify!

class RAYX_API LightSource {
  public:
    LightSource(const DesignObject&);
    virtual ~LightSource() = default;

    // Getter
    std::array<double, 6> getMisalignmentParams() const;
    double getLinear0() const;
    double getLinear45() const;
    double getCircular() const;
    double getVerDivergence() const { return m_verDivergence; }
    double getHorDivergence() const { return m_horDivergence; }
    double getSourceDepth() const { return m_sourceDepth; }
    double getSourceHeight() const { return m_sourceHeight; }
    double getSourceWidth() const { return m_sourceWidth; }

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    double getPhotonEnergy() const;

    double selectEnergy() const;
    glm::dvec3 getDirectionFromAngles(double phi, double psi) const;
    // get the rays according to specific light source, has to be implemented in
    // each class that inherits from LightSource
    virtual std::vector<Ray> getRays() const = 0;

    std::string m_name;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_EnergyDistribution;
    uint32_t m_numberOfRays;

  protected:
    // Geometric Parameters
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // in rad:
    double m_horDivergence;
    double m_verDivergence;

    glm::dmat4x4 m_orientation = glm::dmat4x4();
    glm::dvec4 m_position = glm::dvec4();

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
