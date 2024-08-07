#include "PixelSource.h"

namespace RAYX {

/**
 * get deviation from main ray according to specified distribution
 * (uniform or Thrids for the x, y position))
 * and extent (eg specified width/height of source)
 */
RAYX_FN_ACC
double PixelSource::getPosInDistribution(SourceDist l, double extent, Rand& rand) const {
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
Ray PixelSource::getRay(int32_t lightSourceId, Rand& rand) const {
    double x, y, z, psi, phi, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    x = getPosInDistribution(SourceDist::Thirds, m_sourceWidth, rand);
    x += m_position.x;
    y = getPosInDistribution(SourceDist::Thirds, m_sourceHeight, rand);
    y += m_position.y;
    z = getPosInDistribution(SourceDist::Uniform, m_sourceDepth, rand);
    z += m_position.z;
    en = m_energyDistribution.selectEnergy(rand);  // LightSource.cpp
    // double z = (rn[2] - 0.5) * m_sourceDepth;
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get random deviation from main ray based on divergence
    psi = getPosInDistribution(SourceDist::Uniform, m_verDivergence, rand);
    phi = getPosInDistribution(SourceDist::Uniform, m_horDivergence, rand);
    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto rotation = glm::dmat3(m_orientation);
    const auto field = rotation * m_field;

    return Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
}

}  // namespace RAYX
