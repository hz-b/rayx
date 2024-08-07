#include "PointSource.h"

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "Shader/Constants.h"

namespace RAYX {

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
RAYX_FN_ACC
double getCoord(const SourceDist l, const double extent, Rand& rand) {
    if (l == SourceDist::Uniform) {
        return (rand.randomDouble() - 0.5) * extent;
    } else {
        return rand.randomDoubleNormalDistributed(0, 1) * extent;
    }
}

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
RAYX_FN_ACC
Ray PointSource::getRay(int lightSourceId, Rand& rand) const {
    double x, y, z, psi, phi, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    x = getCoord(m_widthDist, m_sourceWidth, rand) + m_misalignment.m_translationXerror;
    x += m_position.x;
    y = getCoord(m_heightDist, m_sourceHeight, rand) + m_misalignment.m_translationYerror;
    y += m_position.y;
    z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;
    en = m_energyDistribution.selectEnergy(rand);  // LightSource.cpp
    glm::dvec3 position = glm::dvec3(x, y, z);

    // get random deviation from main ray based on distribution
    // TODO correct misalignments?
    psi = getCoord(m_verDist, m_verDivergence, rand) + m_misalignment.m_rotationXerror.rad;
    phi = getCoord(m_horDist, m_horDivergence, rand) + m_misalignment.m_rotationYerror.rad;
    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto rotation = rotationMatrix(direction);
    const auto field = rotation * m_field;

    return Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
}

}  // namespace RAYX
