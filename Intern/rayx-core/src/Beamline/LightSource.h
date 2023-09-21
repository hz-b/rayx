#pragma once
#include <array>
#include <glm.hpp>
#include <string>
#include <vector>

#include "Core.h"
#include "Data/xml.h"
#include "EnergyDistribution.h"
#include "Shared/Ray.h"

namespace RAYX {
enum class SpreadType { WhiteBand, ThreeEnergies };                // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };  // default ET_VALUES
enum class SourceDistType { Simultaneous, HardEdge, Gauss };       // default simultaneously
enum class SourceDist { Uniform, Gaussian };                       // TODO(rudi): unify!
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class EnergySpreadUnit { EU_PERCENT, EU_eV };

class RAYX_API LightSource {
  public:
    LightSource(const DesignObject&);
    virtual ~LightSource() = default;

    // Getter
    Misalignment getMisalignmentParams() const;
    //float getLinear0() const;
    //float getLinear45() const;
    //float getCircular() const;
    //float getVerDivergence() const { return m_verDivergence; }
    float getHorDivergence() const { return m_horDivergence; }
    //float getSourceDepth() const { return m_sourceDepth; }
    float getSourceHeight() const { return m_sourceHeight; }
    float getSourceWidth() const { return m_sourceWidth; }

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    float getPhotonEnergy() const;

    float selectEnergy() const;
    glm::vec3 getDirectionFromAngles(float phi, float psi) const;
    // get the rays according to specific light source, has to be implemented in
    // each class that inherits from LightSource
    virtual std::vector<Ray> getRays() const = 0;

    std::string m_name;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_EnergyDistribution;
    uint32_t m_numberOfRays;

  protected:
    // Geometric Params
    //float m_sourceDepth;
    float m_sourceHeight;
    float m_sourceWidth;
    // in rad:
    float m_horDivergence;
    float m_verDivergence;

    glm::dmat4x4 m_orientation = glm::dmat4x4();
    glm::vec4 m_position = glm::vec4();

  private:
    // User/Design Parameter
    Misalignment m_misalignmentParams;  // x, y, psi, phi

    // Physics Params
    // point source & matrix source
    //float m_linearPol_0;
    //float m_linearPol_45;
    //float m_circularPol;

    // TODO(Jannis): move to children
    //SourceDistType m_sourceDistributionType;
};

}  // namespace RAYX
