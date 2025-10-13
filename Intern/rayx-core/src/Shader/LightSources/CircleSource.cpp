
#include "CircleSource.h"

#include "Design/DesignSource.h"
#include "Shader/Constants.h"

namespace RAYX {

CircleSource::CircleSource(const DesignSource& dSource) : LightSourceBase(dSource) {
    m_pol = dSource.getStokes();

    m_sourceDepth = dSource.getSourceDepth();
    m_sourceHeight = dSource.getSourceHeight();
    m_sourceWidth = dSource.getSourceWidth();

    m_numOfCircles = dSource.getNumOfCircles();
    m_maxOpeningAngle = dSource.getMaxOpeningAngle();
    m_minOpeningAngle = dSource.getMinOpeningAngle();
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
detail::Ray CircleSource::genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                 Rand& __restrict rand) const {
    // create ray with random position and divergence within the given span
    // for width, height, depth
    auto x = (rand.randomDouble() - 0.5) * m_sourceWidth;
    auto y = (rand.randomDouble() - 0.5) * m_sourceHeight;
    auto z = (rand.randomDouble() - 0.5) * m_sourceDepth;

    const auto en = selectEnergy(energyDistribution, rand);

    // double z = (rn[2] - 0.5) * m_sourceDepth;
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get corresponding direction to create circles
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirection(rand);

    const auto electricField = stokesToElectricField(m_pol, glm::dvec3(0, 0, 1), glm::dvec3(0, 1, 0));

    return detail::Ray{
        .position = position,
        .direction = direction,
        .energy = en,
        .optical_path_length = 0.0,
        .electric_field = electricField,
        .rand = std::move(rand),
        .path_id = rayPathIndex,
        .path_event_id = -1,
        .order = 0,
        .object_id = sourceId,
        .source_id = sourceId,
        .event_type = EventType::Emitted,
    };
}

/**
 * calculate directions to form circles on the next element
 * calculations taken from RAY-UI
 */
RAYX_FN_ACC
glm::dvec3 CircleSource::getDirection(Rand& __restrict rand) const {
    double angle = rand.randomDouble() * 2.0 * PI;
    int circle = rand.randomIntInRange(1, m_numOfCircles) - 1;

    double thetabetweencircles = (m_maxOpeningAngle.rad - m_minOpeningAngle.rad) / (m_numOfCircles - 1.0);
    double theta = thetabetweencircles * circle;
    theta = theta + (rand.randomDouble() - 0.5) * m_deltaOpeningAngle.rad + m_minOpeningAngle.rad;

    double al = cos(angle);
    al = al + sin(angle);
    al = al * sin(theta);
    al = al + cos(theta);

    double am = -cos(theta);
    am = am + sin(angle) * sin(theta);

    double an = -cos(angle) * sin(theta);
    an = an + cos(theta);
    an = an + sin(angle) * sin(theta);

    return glm::dvec3(al, am, an);
}

}  // namespace RAYX
