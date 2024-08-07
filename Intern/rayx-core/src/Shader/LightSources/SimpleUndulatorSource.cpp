#include "SimpleUndulatorSource.h"

namespace RAYX {

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
RAYX_FN_ACC
double SimpleUndulatorSource::getCoord(const double extent, Rand& rand) const { return rand.randomDoubleNormalDistributed(0, 1) * extent; }

/**
 * Creates random rays from simple undulator Source
 *
 * @returns list of rays
 */
RAYX_FN_ACC
Ray SimpleUndulatorSource::getRay(int32_t lightSourceId, Rand& rand) const {
    double x, y, z, psi, phi, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    x = getCoord(m_sourceWidth, rand);
    y = getCoord(m_sourceHeight, rand);
    z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;
    en = m_energyDistribution.selectEnergy(rand);  // LightSource.cpp
    glm::dvec3 position = glm::dvec3(x, y, z);

    phi = getCoord(m_horDivergence, rand);
    psi = getCoord(m_verDivergence, rand);
    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto rotation = glm::dmat3(m_orientation);
    const auto field = rotation * m_field;

    return Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
}

double SimpleUndulatorSource::calcUndulatorSigma() const {
    double undulatorSigma;
    if (m_sigmaType == SigmaType::ST_STANDARD) {
        undulatorSigma = sqrt(2 * m_photonWaveLength / 1000 * m_undulatorLength * 1000000) / (2 * PI);  // in µm
    } else if (m_sigmaType == SigmaType::ST_ACCURATE) {
        undulatorSigma = 3.0 / (4 * PI) * sqrt(m_photonWaveLength / 1000 * m_undulatorLength * 1000000);  // in µm
    } else {
        undulatorSigma = 0;
    }
    return undulatorSigma;
}

double SimpleUndulatorSource::calcUndulatorSigmaS() const {
    double undulatorSigmaS;
    if (m_sigmaType == SigmaType::ST_STANDARD) {
        undulatorSigmaS = sqrt(m_photonWaveLength * 1000 / (2 * m_undulatorLength));  // in µrad
    } else if (m_sigmaType == SigmaType::ST_ACCURATE) {
        undulatorSigmaS = 0.53 * sqrt(m_photonWaveLength * 1000 / m_undulatorLength);  // in µrad
    } else {
        undulatorSigmaS = 0;
    }
    return undulatorSigmaS;
}

double SimpleUndulatorSource::getSourceHeight() const {
    double height = sqrt(pow(m_electronSigmaY, 2) + pow(m_undulatorSigma, 2));
    return height / 1000;  // in µm
}

double SimpleUndulatorSource::getSourceWidth() const {
    double width = sqrt(pow(m_electronSigmaX, 2) + pow(m_undulatorSigma, 2));
    return width / 1000;  // in µm
}

double SimpleUndulatorSource::getHorDivergence() const {
    double hordiv = sqrt(pow(m_electronSigmaXs, 2) + pow(m_undulatorSigmaS, 2));
    return hordiv / 1000000;  // in µrad
}

double SimpleUndulatorSource::getVerDivergence() const {
    double verdiv = sqrt(pow(m_electronSigmaYs, 2) + pow(m_undulatorSigmaS, 2));
    return verdiv / 1000000;  // in µrad
}

}  // namespace RAYX
