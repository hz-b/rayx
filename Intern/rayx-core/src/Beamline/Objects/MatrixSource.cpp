#include "MatrixSource.h"

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignSource.h"
#include "Random.h"
#include "Rml/xml.h"
#include "Shader/Constants.h"

namespace RAYX {

MatrixSource::MatrixSource(const DesignSource& dSource)
    : LightSource(dSource),
      m_pol(dSource.getStokes()),
      m_verDivergence(dSource.getVerDivergence()),
      m_sourceDepth(dSource.getSourceDepth()),
      m_sourceHeight(dSource.getSourceHeight()),
      m_sourceWidth(dSource.getSourceWidth()) {
    m_horDivergence = dSource.getHorDivergence();
}

/**
 * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as
 * columns, eg amountOfRays=20 -> 4*4=16, rest (4 rays) same position and
 * direction as first 4) distributed evenly across width & height of source
 * returns vector of rays
 */
std::vector<Ray> MatrixSource::getRays([[maybe_unused]] int thread_count) const {
    RAYX_PROFILE_FUNCTION();

    double x, y, z, psi, phi,
        en;  // x,y,z pos, psi,phi direction cosines, en=energy
    int rmat = int(sqrt(m_numberOfRays));

    std::vector<Ray> returnList;
    returnList.reserve(m_numberOfRays);
    // rayVector.reserve(1048576);
    RAYX_VERB << "create " << rmat << " times " << rmat << " matrix with Matrix Source...";
    // fill the square with rmat1xrmat1 rays

    for (int col = 0; col < rmat; col++) {
        for (int row = 0; row < rmat; row++) {
            double rn = randomDouble();  // in [0, 1]
            x = -0.5 * m_sourceWidth + (m_sourceWidth / (rmat - 1)) * row + getMisalignmentParams().m_translationXerror;
            x += m_position.x;
            y = -0.5 * m_sourceHeight + (m_sourceHeight / (rmat - 1)) * col + getMisalignmentParams().m_translationYerror;
            y += m_position.y;

            z = (rn - 0.5) * m_sourceDepth;
            z += m_position.z;
            en = selectEnergy();
            glm::dvec3 position = glm::dvec3(x, y, z);

            phi = -0.5 * m_horDivergence + (m_horDivergence / (rmat - 1)) * row + getMisalignmentParams().m_rotationXerror.rad;

            psi = -0.5 * m_verDivergence + (m_verDivergence / (rmat - 1)) * col + getMisalignmentParams().m_rotationYerror.rad;

            glm::dvec3 direction = getDirectionFromAngles(phi, psi);
            glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
            direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

            const auto field = stokesToElectricField(m_pol, m_orientation);

            Ray r = {position, EventType::Emitted, direction, en, field, 0.0, 0, -1, -1};

            returnList.push_back(r);
        }
    }
    // afterwards start from the beginning again
    for (uint32_t i = 0; i < m_numberOfRays - rmat * rmat; i++) {
        /*Ray r = rayVector.at(i);
        glm::dvec3 position =
            glm::dvec3(r.m_position.x, r.m_position.y, r.m_position.z);
        glm::dvec3 direction =
            glm::dvec3(r.m_position.x, r.m_position.y, r.m_position.z);
        // selects the energy of the ray based on the given distribution or .dat
        // file
        en = selectEnergy();
        // stokes parameters, needed for efficiency calculations (whether a ray
        // is absorbed or reflected), related to polarisation
        glm::dvec4 stokes =
            glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r_copy(position.x, position.y, position.z, direction.x, direction.y,
                   direction.z, stokes.x, stokes.y, stokes.z, stokes.w, en,
                   1.0);*/
        Ray r_copy((const Ray&)returnList.at(i));
        r_copy.m_energy = en = selectEnergy();
        returnList.push_back(r_copy);
    }
    return returnList;
}

}  // namespace RAYX
