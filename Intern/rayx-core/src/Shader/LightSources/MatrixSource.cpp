#include "MatrixSource.h"

namespace RAYX {

RAYX_FN_ACC
Ray MatrixSource::getRay(int rayId, int lightSourceId, Rand& rand) const {
    double x, y, z, psi, phi, en;  // x,y,z pos, psi,phi direction cosines, en=energy
    int rmat = int(sqrt(float(m_numberOfRays)));

    rayId = rayId % (rmat * rmat);
    const auto row = rayId % rmat;
    const auto col = rayId / rmat;

    // double rn = squaresDoubleRNG(inv.ctr);  // in [0, 1]
    double rn = 0.5;
    x = -0.5 * m_sourceWidth + (m_sourceWidth / (rmat - 1)) * row + m_misalignment.m_translationXerror;
    x += m_position.x;
    y = -0.5 * m_sourceHeight + (m_sourceHeight / (rmat - 1)) * col + m_misalignment.m_translationYerror;
    y += m_position.y;

    z = (rn - 0.5) * m_sourceDepth;
    z += m_position.z;
    en = m_energyDistribution.selectEnergy(rand);
    glm::dvec3 position = glm::dvec3(x, y, z);

    phi = -0.5 * m_horDivergence + (m_horDivergence / (rmat - 1)) * row + m_misalignment.m_rotationXerror.rad;
    psi = -0.5 * m_verDivergence + (m_verDivergence / (rmat - 1)) * col + m_misalignment.m_rotationYerror.rad;

    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto rotation = glm::dmat3(m_orientation);
    const auto field = rotation * m_field;

    auto r = Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
    return r;
}

}  // namespace RAYX
