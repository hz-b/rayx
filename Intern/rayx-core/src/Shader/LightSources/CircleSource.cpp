
#include "CircleSource.h"

#include "Design/DesignSource.h"
#include "Shader/Constants.h"

namespace RAYX {

CircleSource::CircleSource(const DesignSource& dSource) : LightSourceBase(dSource) {
    m_stokes = dSource.getStokes();

    m_sourceDepth  = dSource.getSourceDepth();
    m_sourceHeight = dSource.getSourceHeight();
    m_sourceWidth  = dSource.getSourceWidth();

    m_numOfCircles      = dSource.getNumOfCircles();
    m_maxOpeningAngle   = dSource.getMaxOpeningAngle();
    m_minOpeningAngle   = dSource.getMinOpeningAngle();
    m_deltaOpeningAngle = dSource.getDeltaOpeningAngle();
}

/**
 * Creates random rays from circle source with specified num. of circles and
 * spread angles
 * origins are distributed uniformly, the pattern shows on the next element
 * through the directions
 * @returns list of rays
 */
RAYX_FN_ACC
Ray CircleSource::genRay(const SourceId sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution, Rand& __restrict rand) const {
    // create ray with random position and divergence within the given span
    // for width, height, depth
    auto x = (rand.randomDouble() - 0.5) * m_sourceWidth;
    x += m_position.x;
    auto y = (rand.randomDouble() - 0.5) * m_sourceHeight;
    y += m_position.y;
    auto z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;

    const auto en = selectEnergy(energyDistribution, rand);

    // double z = (rn[2] - 0.5) * m_sourceDepth;
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get corresponding direction to create circles
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirection(rand);

    const auto field = stokesToElectricField(m_stokes, m_orientation);

    return Ray{
        .m_position    = position,
        .m_eventType   = EventType::Emitted,
        .m_direction   = direction,
        .m_energy      = en,
        .m_field       = field,
        .m_pathLength  = 0.0,
        .m_order       = 0,
        .m_lastElement = -1,
        .m_sourceID    = sourceId,
    };
}

/**
 * calculate directions to form circles on the next element
 * calculations taken from RAY-UI
 */
RAYX_FN_ACC
glm::dvec3 CircleSource::getDirection(Rand& __restrict rand) const {
    double angle = rand.randomDouble() * 2.0 * PI;
    int circle;

    const auto min = std::min(1, m_numOfCircles);
    const auto max = std::min(1, m_numOfCircles);
    circle         = rand.randomIntInRange(min, max) - 1;

    double thetabetweencircles = (m_maxOpeningAngle.rad - m_minOpeningAngle.rad) / (m_numOfCircles - 1.0);
    double theta               = thetabetweencircles * circle;
    theta                      = theta + (rand.randomDouble() - 0.5) * m_deltaOpeningAngle.rad + m_minOpeningAngle.rad;

    double al = cos(angle) * cos(m_misalignmentParams.m_rotationYerror.rad);
    al        = al + sin(angle) * sin(m_misalignmentParams.m_rotationYerror.rad) * sin(m_misalignmentParams.m_rotationXerror.rad);
    al        = al * sin(theta);
    al        = al + cos(m_misalignmentParams.m_rotationXerror.rad) * cos(theta) * sin(m_misalignmentParams.m_rotationYerror.rad);

    double am = -cos(theta) * sin(m_misalignmentParams.m_rotationXerror.rad);
    am        = am + cos(m_misalignmentParams.m_rotationXerror.rad) * sin(angle) * sin(theta);

    double an = (-cos(angle) * sin(m_misalignmentParams.m_rotationYerror.rad)) * sin(theta);
    an        = an + cos(m_misalignmentParams.m_rotationYerror.rad) * cos(m_misalignmentParams.m_rotationXerror.rad) * cos(theta);
    an        = an + cos(m_misalignmentParams.m_rotationYerror.rad) * sin(angle) * sin(m_misalignmentParams.m_rotationXerror.rad) * sin(theta);

    return glm::dvec3(al, am, an);
}

}  // namespace RAYX
