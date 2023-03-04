#include "PointSource.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Random.h"

namespace RAYX {

PointSource::PointSource(const DesignObject& dobj) : LightSource(dobj) {
    m_widthDist = dobj.parseSourceWidthDistribution();
    m_heightDist = dobj.parseSourceHeightDistribution();
    m_horDist = dobj.parseHorDivDistribution();
    m_verDist = dobj.parseVerDivDistribution();
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
std::vector<Ray> PointSource::getRays() const {
    double x, y, z, psi, phi,
        en;  // x,y,z pos, psi,phi direction cosines, en=energy

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    // rayList.reserve(1048576);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    for (int i = 0; i < n; i++) {
        x = getCoord(m_widthDist, m_sourceWidth) + getMisalignmentParams()[0];
        x += m_position.x;
        y = getCoord(m_heightDist, m_sourceHeight) + getMisalignmentParams()[1];
        y += m_position.y;
        z = (randomDouble() - 0.5) * m_sourceDepth;
        z += m_position.z;
        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        psi = getCoord(m_verDist, m_verDivergence) + getMisalignmentParams()[2];
        phi = getCoord(m_horDist, m_horDivergence) + getMisalignmentParams()[3];
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
        direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);
        glm::dvec4 stokes = glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r = {position, W_UNINIT, direction, en, stokes, 0.0, 0.0, 0.0, 0.0};

        rayList.push_back(r);
    }
    return rayList;
}

}  // namespace RAYX
