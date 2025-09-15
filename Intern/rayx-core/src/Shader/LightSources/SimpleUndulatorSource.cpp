#include "SimpleUndulatorSource.h"

#include "Design/DesignSource.h"
#include "Shader/Utils.h"

namespace RAYX {
namespace {

double calcUndulatorSigma(const SigmaType sigmaType, const double photonWaveLength, const double undulatorLength) {
    double undulatorSigma;
    if (sigmaType == SigmaType::ST_STANDARD) {
        undulatorSigma = sqrt(2 * photonWaveLength / 1000 * undulatorLength * 1000000) / (2 * PI);  // in µm
    } else if (sigmaType == SigmaType::ST_ACCURATE) {
        undulatorSigma = 3.0 / (4 * PI) * sqrt(photonWaveLength / 1000 * undulatorLength * 1000000);  // in µm
    } else {
        undulatorSigma = 0;
    }
    return undulatorSigma;
}

double calcUndulatorSigmaS(const SigmaType sigmaType, const double photonWaveLength, const double undulatorLength) {
    double undulatorSigmaS;
    if (sigmaType == SigmaType::ST_STANDARD) {
        undulatorSigmaS = sqrt(photonWaveLength * 1000 / (2 * undulatorLength));  // in µrad
    } else if (sigmaType == SigmaType::ST_ACCURATE) {
        undulatorSigmaS = 0.53 * sqrt(photonWaveLength * 1000 / undulatorLength);  // in µrad
    } else {
        undulatorSigmaS = 0;
    }
    return undulatorSigmaS;
}

double getHorDivergence(const double electronSigmaXs, const double undulatorSigmaS) {
    double hordiv = sqrt(pow(electronSigmaXs, 2) + pow(undulatorSigmaS, 2));
    return hordiv / 1000000;  // in µrad
}

double getVerDivergence(const double electronSigmaYs, const double undulatorSigmaS) {
    double verdiv = sqrt(pow(electronSigmaYs, 2) + pow(undulatorSigmaS, 2));
    return verdiv / 1000000;  // in µrad
}

double getSourceWidth(const double electronSigmaX, const double undulatorSigma) {
    double width = sqrt(pow(electronSigmaX, 2) + pow(undulatorSigma, 2));
    return width / 1000;  // in µm
}

double getSourceHeight(const double electronSigmaY, const double undulatorSigma) {
    double height = sqrt(pow(electronSigmaY, 2) + pow(undulatorSigma, 2));
    return height / 1000;  // in µm
}

}  // unnamed namespace

SimpleUndulatorSource::SimpleUndulatorSource(const DesignSource& dSource) : LightSourceBase(dSource) {
    const auto sigmaType       = dSource.getSigmaType();
    const auto undulatorLength = dSource.getUndulatorLength();
    // const auto photonEnergy = dSource.getEnergy();
    const auto photonWaveLength = energyToWaveLength(m_photonEnergy);
    const auto electronSigmaX   = dSource.getElectronSigmaX();
    const auto electronSigmaXs  = dSource.getElectronSigmaXs();
    const auto electronSigmaY   = dSource.getElectronSigmaY();
    const auto electronSigmaYs  = dSource.getElectronSigmaYs();
    const auto undulatorSigma   = calcUndulatorSigma(sigmaType, photonWaveLength, undulatorLength);
    const auto undulatorSigmaS  = calcUndulatorSigmaS(sigmaType, photonWaveLength, undulatorLength);

    m_sourceDepth   = dSource.getSourceDepth();
    m_pol           = dSource.getStokes();
    m_horDivergence = getHorDivergence(electronSigmaXs, undulatorSigmaS);
    m_verDivergence = getVerDivergence(electronSigmaYs, undulatorSigmaS);
    m_sourceWidth   = getSourceWidth(electronSigmaX, undulatorSigma);
    m_sourceHeight  = getSourceHeight(electronSigmaY, undulatorSigma);
}

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
RAYX_FN_ACC
double SimpleUndulatorSource::getCoord(const double extent, Rand& __restrict rand) const { return rand.randomDoubleNormalDistributed(0, 1) * extent; }

/**
 * Creates random rays from simple undulator Source
 *
 * @returns list of rays
 */
RAYX_FN_ACC
Ray SimpleUndulatorSource::genRay(const int rayPathIndex, const int sourceId, const EnergyDistributionDataVariant& __restrict energyDistribution,
                                  Rand& __restrict rand) const {
    // create ray with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    auto x = getCoord(m_sourceWidth, rand);
    auto y = getCoord(m_sourceHeight, rand);
    auto z = (rand.randomDouble() - 0.5) * m_sourceDepth;
    z += m_position.z;
    const auto en       = selectEnergy(energyDistribution, rand);
    glm::dvec3 position = glm::dvec3(x, y, z);

    const auto phi = getCoord(m_horDivergence, rand);
    const auto psi = getCoord(m_verDivergence, rand);
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
