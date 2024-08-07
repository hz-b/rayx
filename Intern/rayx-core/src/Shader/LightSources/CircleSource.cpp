#include "Shader/LightSources.h"

namespace RAYX {

/**
 * calculate directions to form circles on the next element
 * calculations taken from RAY-UI
 */
RAYX_FN_ACC
glm::dvec3 CircleSource::getDirection(Rand& rand) const {
    double angle = rand.randomDouble() * 2.0 * RAYX::PI;
    int circle;

    circle = rand.randomIntInRange(1, m_numOfCircles) - 1;

    double thetabetweencircles = (m_maxOpeningAngle.rad - m_minOpeningAngle.rad) / (m_numOfCircles - 1.0);
    double theta = thetabetweencircles * circle;
    theta = theta + (rand.randomDouble() - 0.5) * m_deltaOpeningAngle.rad + m_minOpeningAngle.rad;

    double al = cos(angle) * cos(m_misalignment.m_rotationYerror.rad);
    al = al + sin(angle) * sin(m_misalignment.m_rotationYerror.rad) * sin(m_misalignment.m_rotationXerror.rad);
    al = al * sin(theta);
    al = al + cos(m_misalignment.m_rotationXerror.rad) * cos(theta) * sin(m_misalignment.m_rotationYerror.rad);

    double am = -cos(theta) * sin(m_misalignment.m_rotationXerror.rad);
    am = am + cos(m_misalignment.m_rotationXerror.rad) * sin(angle) * sin(theta);

    double an = (-cos(angle) * sin(m_misalignment.m_rotationYerror.rad)) * sin(theta);
    an = an + cos(m_misalignment.m_rotationYerror.rad) * cos(m_misalignment.m_rotationXerror.rad) * cos(theta);
    an = an + cos(m_misalignment.m_rotationYerror.rad) * sin(angle) * sin(m_misalignment.m_rotationXerror.rad) * sin(theta);

    return glm::dvec3(al, am, an);
}

/**
 * Creates random rays from circle source with specified num. of circles and
 * spread angles
 * origins are distributed uniformly, the pattern shows on the next element
 * through the directions
 * @returns list of rays
 */
RAYX_FN_ACC
Ray CircleSource::getRay(int lightSourceId, Rand& rand) const {
    double x, y, z, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    x = (rand.randomDouble() - 0.5) * m_sourceWidth;
    x += m_position.x;
    y = (rand.randomDouble() - 0.5) * m_sourceHeight;
    y += m_position.y;
    z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;

    en = m_energyDistribution.selectEnergy(rand);
    // double z = (rn[2] - 0.5) * m_sourceDepth;
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get corresponding direction to create circles
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirection(rand);

    const auto rotation = glm::dmat3(m_orientation);
    const auto field = rotation * m_field;

    return Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
}

}  // namespace RAYX
