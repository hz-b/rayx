#include "PointSource.h"

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Random.h"
#include "Shader/Constants.h"

namespace RAYX {

PointSource::PointSource(const DesignSource& dSource)
    : LightSource(dSource),
      m_widthDist(dSource.getWidthDist()),
      m_heightDist(dSource.getHeightDist()),
      m_horDist(dSource.getHorDist()),
      m_verDist(dSource.getVerDist()),
      m_pol(dSource.getStokes()),
      m_verDivergence(dSource.getVerDivergence()),
      m_sourceDepth(dSource.getSourceDepth()),
      m_sourceHeight(dSource.getSourceHeight()),
      m_sourceWidth(dSource.getSourceWidth()) {
    m_horDivergence = dSource.getHorDivergence();
}

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
double getCoord(const SourceDist l, const double extent) {
    if (l == SourceDist::Uniform) {
        return (randomDouble() - 0.5) * extent;
    } else {
        return randomNormal(0, 1) * extent;
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
std::vector<Ray> PointSource::getRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION();

    /**
     * initialize parallelization when counter is positive
     * with special OMP use case for num_threads(1)
     * */
    if (thread_count == 0) {
        thread_count = 1;
#define DIPOLE_OMP
    } else if (thread_count > 1) {
#define DIPOLE_OMP
    }

    double x, y, z, psi, phi, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

// create n rays with random position and divergence within the given span
// for width, height, depth, horizontal and vertical divergence
#if defined(DIPOLE_OMP)
#pragma omp parallel for num_threads(thread_count)
#endif
    for (int i = 0; i < n; i++) {
        x = getCoord(m_widthDist, m_sourceWidth) + getMisalignmentParams().m_translationXerror;
        x += m_position.x;
        y = getCoord(m_heightDist, m_sourceHeight) + getMisalignmentParams().m_translationYerror;
        y += m_position.y;
        z = (randomDouble() - 0.5) * m_sourceDepth;
        z += m_position.z;
        en = selectEnergy();  // LightSource.cpp
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        // TODO correct misalignments?
        psi = getCoord(m_verDist, m_verDivergence) + getMisalignmentParams().m_rotationXerror.rad;
        phi = getCoord(m_horDist, m_horDivergence) + getMisalignmentParams().m_rotationYerror.rad;
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
        direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

        // const auto rotation = rotationMatrix(direction);
        const auto field = /* rotation *  */ stokesToElectricField(m_pol);

        Ray r = {position, EventType::Emitted, direction, en, field, 0.0, 0, -1, -1};
#if defined(DIPOLE_OMP)
#pragma omp critical
        {
            rayList.push_back(r);
        }
#else
        rayList.push_back(r);
#endif
    }
    return rayList;
}

}  // namespace RAYX
