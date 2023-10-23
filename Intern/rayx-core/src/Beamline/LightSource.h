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
enum class SpreadType { HardEdge, SoftEdge, SeperateEnergies };  // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };         // default ET_VALUES
enum class SourceDistType { Simultaneous, HardEdge, Gauss };              // default simultaneously
enum class SourceDist { Uniform, Gaussian, Thirds, Circle };                      // TODO(rudi): unify! ('Thrids' represents PixelSource Footprint)
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class EnergySpreadUnit { EU_PERCENT, EU_eV };

class RAYX_API LightSource {
  public:
    LightSource(const DesignObject&);
    virtual ~LightSource() = default;

    // Setter (Only used in frontend)
    void setSourceHeight(double height) { m_sourceHeight = height; }
    void setSourceWidth(double width) { m_sourceWidth = width; }

    // Getter
    Misalignment getMisalignmentParams() const;
    double getHorDivergence() const { return m_horDivergence; }
    double getSourceHeight() const { return m_sourceHeight; }
    double getSourceWidth() const { return m_sourceWidth; }

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    double getPhotonEnergy() const;

    double selectEnergy() const;
    static glm::dvec3 getDirectionFromAngles(double phi, double psi);
    // get the rays according to specific light source, has to be implemented in
    // each class that inherits from LightSource
    virtual std::vector<Ray> getRays(int thread_count = 0) const = 0;

    std::string m_name;

    /** the energy distribution used when deciding the energies of the rays. */
    EnergyDistribution m_EnergyDistribution;
    uint32_t m_numberOfRays;

  protected:
    // Geometric Params

    double m_sourceHeight;
    double m_sourceWidth;

    // in rad:
    double m_horDivergence;  // phi
    double m_verDivergence;  // psi

    glm::dmat4x4 m_orientation = glm::dmat4x4();
    glm::dvec4 m_position = glm::dvec4();

  private:
    // User/Design Parameter
    Misalignment m_misalignmentParams;  // x, y, psi, phi
};

}  // namespace RAYX
