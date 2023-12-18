#include "DesignLightSourceType.h"
#include "DesignLightSource.h"
#include "Debug/Debug.h"
#include "Random.h"

namespace RAYX {
std::vector<Ray> DesignLightSourceType::getRays(const DesignLightSource& s) const {
    return visit([&](const auto& lst) {
        return lst.getRays(s);
    });
}

glm::dvec3 getDirectionFromAngles_copy(double phi, double psi) {
    double al = cos(psi) * sin(phi);
    double am = -sin(psi);
    double an = cos(psi) * cos(phi);
    return {al, am, an};
}

std::vector<Ray> DesignMatrixSource::getRays(const DesignLightSource& s) const {
    uint32_t m_numberOfRays = s.field<NumberOfRaysStr>();
    DesignMisalignment misalignment = s.field<MisalignmentStr>();

    double x, y, z, psi, phi;
    int rmat = int(sqrt(m_numberOfRays));

    double m_sourceWidth = s.field<SourceWidthStr>();
    double m_sourceHeight = s.field<SourceHeightStr>();
    double m_sourceDepth = this->field<SourceDepthStr>();
    double m_horDivergence = s.field<HorDivergenceStr>();
    double m_verDivergence = s.field<VerDivergenceStr>();

    // TODO init these!
    glm::dvec3 m_position;
    glm::dmat4x4 m_orientation;

    std::vector<Ray> returnList;
    returnList.reserve(m_numberOfRays);
    RAYX_VERB << "create " << rmat << " times " << rmat << " matrix with Matrix Source...";

    for (int col = 0; col < rmat; col++) {
        for (int row = 0; row < rmat; row++) {
            double rn = randomDouble();  // in [0, 1]
            x = -0.5 * m_sourceWidth + (m_sourceWidth / (rmat - 1)) * row + misalignment.field<TranslationXErrorStr>();
            x += m_position.x;
            y = -0.5 * m_sourceHeight + (m_sourceHeight / (rmat - 1)) * col + misalignment.field<TranslationYErrorStr>();
            y += m_position.y;

            z = (rn - 0.5) * m_sourceDepth;
            z += m_position.z;
            double en = s.field<EnergyDistributionStr>().selectEnergy();
            glm::dvec3 position = glm::dvec3(x, y, z);

            phi = -0.5 * m_horDivergence + (m_horDivergence / (rmat - 1)) * row + misalignment.field<RotationXErrorStr>();

            psi = -0.5 * m_verDivergence + (m_verDivergence / (rmat - 1)) * col + misalignment.field<RotationYErrorStr>();

            glm::dvec3 direction = getDirectionFromAngles_copy(phi, psi);
            glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
            direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);
            glm::dvec4 stokes = glm::dvec4(1, field<LinearPol0Str>(), field<LinearPol45Str>(), field<CircularPolStr>());

            Ray r = {position, ETYPE_UNINIT, direction, en, stokes, 0.0, 0.0, -1.0, -1.0};
            // Ray(1, 2, 3, 7, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16);

            returnList.push_back(r);
        }
    }

    // afterwards start from the beginning again
    for (uint32_t i = 0; i < m_numberOfRays - rmat * rmat; i++) {
        Ray r_copy((const Ray&)returnList.at(i));
        r_copy.m_energy = s.field<EnergyDistributionStr>().selectEnergy();
        returnList.push_back(r_copy);
    }
    return returnList;
}

}
