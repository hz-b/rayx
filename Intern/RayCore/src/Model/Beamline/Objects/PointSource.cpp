#include "PointSource.h"

#include <Data/xml.h>

#include <utility>

#include "Debug.h"

namespace RAYX {

PointSource::PointSource(const DesignObject& dobj) : LightSource(dobj) {
    m_widthDist = dobj.parseSourceWidthDistribution();
    m_heightDist = dobj.parseSourceHeightDistribution();
    m_horDist = dobj.parseHorDivDistribution();
    m_verDist = dobj.parseVerDivDistribution();
}

struct RandomState {
    RandomState() : m_uniformDist(0, 1), m_normDist(0, 1) {}

    std::uniform_real_distribution<double> m_uniformDist;
    std::normal_distribution<double> m_normDist;
    std::default_random_engine m_randEngine;
};

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
double getCoord(const SourceDist l, const double extent, RandomState& rs) {
    if (l == SourceDist::Uniform) {
        return (rs.m_uniformDist(rs.m_randEngine) - 0.5) * extent;
    } else {
        return (rs.m_normDist(rs.m_randEngine) * extent);
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

    RandomState rs;

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    // rayList.reserve(1048576);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    for (int i = 0; i < n; i++) {
        x = getCoord(m_widthDist, m_sourceWidth, rs) + getMisalignmentParams()[0];
        y = getCoord(m_heightDist, m_sourceHeight, rs) + getMisalignmentParams()[1];
        z = (rs.m_uniformDist(rs.m_randEngine) - 0.5) * m_sourceDepth;
        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        psi = getCoord(m_verDist, m_verDivergence, rs) + getMisalignmentParams()[2];
        phi = getCoord(m_horDist, m_horDivergence, rs) + getMisalignmentParams()[3];
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 stokes = glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r = {position, 1.0, direction, en, stokes, 0.0, 0.0, 0.0, 0.0};

        rayList.push_back(r);
    }
    return rayList;
}

}  // namespace RAYX
