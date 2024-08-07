#pragma once

#include "Beamline/Misalignment.h"
#include "Data/Strings.h"
#include "Design/DesignSource.h"
#include "EnergyDistribution.h"
#include "Shader/EventType.h"
#include "Shader/Rand.h"
#include "Shader/Ray.h"

namespace RAYX {

enum class SpreadType { HardEdge, SoftEdge, SeparateEnergies };    // default WhiteBand
enum class EnergyDistributionType { File, Values, Total, Param };  // default ET_VALUES
enum class SourceDist { Uniform, Gaussian, Thirds, Circle };       // SourceDist::Thirds represents PixelSource Footprint
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class EnergySpreadUnit { EU_PERCENT, EU_eV };
enum class SigmaType { ST_STANDARD, ST_ACCURATE };

struct LightSource {
    LightSource() = default;
    inline LightSource(const DesignSource& designSource);

    uint32_t m_numberOfRays;

    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;

    double m_horDivergence;  // phi in rad
    double m_verDivergence;  // psi in rad

    glm::dmat4 m_orientation;
    glm::dvec4 m_position;

    Misalignment m_misalignment;  // x, y, psi, phi
};

LightSource::LightSource(const DesignSource& designSource)
    : m_numberOfRays(static_cast<int32_t>(designSource.getNumberOfRays())),
      m_sourceDepth(designSource.getSourceDepth()),
      m_sourceHeight(designSource.getSourceHeight()),
      m_sourceWidth(designSource.getSourceWidth()),
      m_horDivergence(designSource.getHorDivergence()),
      m_verDivergence(designSource.getVerDivergence()),
      m_orientation(designSource.getWorldOrientation()),
      m_position(designSource.getWorldPosition()),
      m_misalignment(designSource.getMisalignment()) {}

RAYX_FN_ACC inline glm::dvec3 getDirectionFromAngles(const double phi, const double psi) {
    double al = glm::cos(psi) * glm::sin(phi);
    double am = -glm::sin(psi);
    double an = glm::cos(psi) * glm::cos(phi);
    return {al, am, an};
}

RAYX_FN_ACC inline double calcPhotonWavelength(double photonEnergy) {
    // Energy Distribution Type : Values only
    double photonWaveLength = photonEnergy == 0.0 ? 0 : INV_NM_TO_EVOLT / photonEnergy;  // i nm to eV
    return photonWaveLength;
}

}  // namespace RAYX
