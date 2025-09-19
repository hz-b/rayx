#include "PixelSource.h"

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Shader/Constants.h"

namespace RAYX {

PixelSource::PixelSource(const DesignSource& deso)
    : LightSourceBase(deso),
      m_pol(deso.getStokes()),
      m_horDivergence(deso.getHorDivergence()),
      m_verDivergence(deso.getVerDivergence()),
      m_sourceDepth(deso.getSourceDepth()),
      m_sourceHeight(deso.getSourceHeight()),
      m_sourceWidth(deso.getSourceWidth()) {}

/**
 * get deviation from main ray according to specified distribution
 * (uniform or Thrids for the x, y position))
 * and extent (eg specified width/height of source)
 */
RAYX_FN_ACC
double getPosInDistribution(SourceDist l, double extent, Rand& __restrict rand) {
    if (l == SourceDist::Uniform) {
        return (rand.randomDouble() - 0.5) * extent;
    } else if (l == SourceDist::Thirds) {
        double temp = (rand.randomDouble() - 0.5) * 2 / 3 * extent;
        return temp + copysign(1.0, temp) * 1 / 6 * extent;
    } else {
        return 0;
    }
}

/**
 * Creates random rays from pixel source with specified distributed width
 * & height in 4 distinct pixels
 * position and directions are distributed uniform
 *
 * @returns list of rays
 */
RAYX_FN_ACC
Ray PixelSource::genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                        Rand& __restrict rand) const {
    // create ray with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    auto x        = getPosInDistribution(SourceDist::Thirds, m_sourceWidth, rand);
    auto y        = getPosInDistribution(SourceDist::Thirds, m_sourceHeight, rand);
    auto z        = getPosInDistribution(SourceDist::Uniform, m_sourceDepth, rand);
    const auto en = selectEnergy(energyDistribution, rand);
    // double z = (rn[2] - 0.5) * m_sourceDepth;
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get random deviation from main ray based on divergence
    const auto psi = getPosInDistribution(SourceDist::Uniform, m_verDivergence, rand);
    const auto phi = getPosInDistribution(SourceDist::Uniform, m_horDivergence, rand);
    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psi);

    const auto electricField = stokesToElectricField(m_pol, glm::dvec3(0, 0, 1), glm::dvec3(0, 1, 0));

    return Ray{
        .position            = position,
        .direction           = direction,
        .energy              = en,
        .optical_path_length = 0.0,
        .electric_field      = electricField,
        .rand                = std::move(rand),
        .path_id             = rayPathIndex,
        .path_event_id       = 0,
        .order               = 0,
        .object_id           = sourceId,
        .source_id           = sourceId,
        .event_type          = EventType::Emitted,
    };
}

}  // namespace RAYX
