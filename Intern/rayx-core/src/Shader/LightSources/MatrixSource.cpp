#include "MatrixSource.h"

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignSource.h"
#include "Random.h"
#include "Rml/xml.h"
#include "Shader/Constants.h"

namespace RAYX {

MatrixSource::MatrixSource(const DesignSource& dSource)
    : LightSourceBase(dSource),
      m_pol(dSource.getStokes()),
      m_horDivergence(dSource.getHorDivergence()),
      m_verDivergence(dSource.getVerDivergence()),
      m_sourceDepth(dSource.getSourceDepth()),
      m_sourceHeight(dSource.getSourceHeight()),
      m_sourceWidth(dSource.getSourceWidth()) {}

/**
 * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as
 * columns, eg amountOfRays=20 -> 4*4=16, rest (4 rays) same position and
 * direction as first 4) distributed evenly across width & height of source
 * returns vector of rays
 */
RAYX_FN_ACC
Ray MatrixSource::genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                         Rand& __restrict rand) const {
    // Calculate grid size
    const int rmat  = int(std::sqrt(m_numberOfRays));
    const int nGrid = rmat * rmat;
    const int row   = rayPathIndex % rmat;
    const int col   = (rayPathIndex / rmat) % rmat;

    // Count how many rays share this origin
    int originIndex   = row + rmat * col;
    int raysPerOrigin = m_numberOfRays / nGrid;
    int extraRays     = m_numberOfRays % nGrid;
    // The first 'extraRays' origins get one extra ray
    int nRaysThisOrigin = raysPerOrigin + (originIndex < extraRays ? 1 : 0);

    double rn = rand.randomDouble();  // in [0, 1]
    auto x    = -0.5 * m_sourceWidth + (m_sourceWidth / (rmat - 1)) * row + m_misalignmentParams.m_translationXerror;
    x += m_position.x;
    auto y = -0.5 * m_sourceHeight + (m_sourceHeight / (rmat - 1)) * col + m_misalignmentParams.m_translationYerror;
    y += m_position.y;

    auto z = (rn - 0.5) * m_sourceDepth;
    z += m_position.z;
    const auto en       = selectEnergy(energyDistribution, rand);
    glm::dvec3 position = glm::dvec3(x, y, z);

    const auto phi = -0.5 * m_horDivergence + (m_horDivergence / (rmat - 1)) * row + m_misalignmentParams.m_rotationXerror.rad;
    const auto psi = -0.5 * m_verDivergence + (m_verDivergence / (rmat - 1)) * col + m_misalignmentParams.m_rotationYerror.rad;

    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir   = m_orientation * glm::dvec4(direction, 0.0);
    direction            = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    // Scale the electic field so that each origin emits the same total intensity
    auto electricField = stokesToElectricField(m_pol, m_orientation);
    electricField /= static_cast<double>(nRaysThisOrigin);

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
