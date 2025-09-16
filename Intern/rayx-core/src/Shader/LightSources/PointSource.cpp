#include "PointSource.h"

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Random.h"
#include "Shader/Constants.h"

namespace RAYX {
namespace {

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
RAYX_FN_ACC
double getCoord(const SourceDist l, const double extent, Rand& __restrict rand) {
    if (l == SourceDist::Uniform) {
        return (rand.randomDouble() - 0.5) * extent;
    } else {
        return rand.randomDoubleNormalDistributed(0, 1) * extent;
    }
}

}  // unnamed namespace

PointSource::PointSource(const DesignSource& dSource)
    : LightSourceBase(dSource),
      m_widthDist(dSource.getWidthDist()),
      m_heightDist(dSource.getHeightDist()),
      m_horDist(dSource.getHorDist()),
      m_verDist(dSource.getVerDist()),
      m_pol(dSource.getStokes()),
      m_horDivergence(dSource.getHorDivergence()),
      m_verDivergence(dSource.getVerDivergence()),
      m_sourceDepth(dSource.getSourceDepth()),
      m_sourceHeight(dSource.getSourceHeight()),
      m_sourceWidth(dSource.getSourceWidth()) {}

/**
 * Creates random rays from point source with specified width and height
 * distributed according to either uniform or gaussian distribution across width
 * & height of source the deviation of the direction of each ray from the main
 * ray (0,0,1, phi=psi=0) can also be specified to be uniform or gaussian within
 * a given range (m_verDivergence, m_horDivergence) z-position of ray is always
 * from uniform distribution
 *
 * @returns list of rays
 */
RAYX_FN_ACC Ray PointSource::genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                    Rand& __restrict rand) const {
    // create ray with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    auto x = getCoord(m_widthDist, m_sourceWidth, rand) + m_misalignmentParams.m_translationXerror;
    x += m_position.x;
    auto y = getCoord(m_heightDist, m_sourceHeight, rand) + m_misalignmentParams.m_translationYerror;
    y += m_position.y;
    auto z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;
    const auto en       = selectEnergy(energyDistribution, rand);
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get random deviation from main ray based on distribution
    // TODO correct misalignments?
    const auto psi = getCoord(m_verDist, m_verDivergence, rand) + m_misalignmentParams.m_rotationXerror.rad;
    const auto phi = getCoord(m_horDist, m_horDivergence, rand) + m_misalignmentParams.m_rotationYerror.rad;
    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir   = m_orientation * glm::dvec4(direction, 0.0);
    direction            = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto electricField = stokesToElectricField(m_pol, m_orientation);

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
