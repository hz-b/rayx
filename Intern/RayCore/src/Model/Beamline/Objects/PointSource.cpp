#include "PointSource.h"

#include <Data/xml.h>

#include <utility>

#include "Debug.h"

namespace RAYX {

/**
 * @param name              name of source
 * @param dist              Energy Distribution object, describes which values
 * the energy of a ray can take. this energy is then chosen randomly for each
 * ray from this distribution
 * @param sourceWidth       width of the source, the x-coordinate of the
 * origin(position) of the ray will be within [-sourceWidth/2, +sourceWidth/2]
 * @param sourceHeight      height of the source, the y-coordinate of the
 * origin(position) of the ray will be within [-sourceHeight/2, +sourceHeight/2]
 * @param sourceDepth       depth of the source, the z-coordinate of the
 * origin(position) of the ray will be within [-sourceDepth/2, +sourceDepth/2],
 * but chosen randomly. if set to 0, the rays are chosen deterministically,
 * which is useful for testing
 * @param horDivergence     divergence span in mrad of the x-direction of the
 * ray.
 * @param verDivergence     divergence span in mrad of the y-direction of the
 * ray, since direction vectors have unit length, the z-direction is derived
 * from the two other angles
 * @param widthDist         distribution for the x-coordinate of the
 * origin(position) 0 = uniform, 1 = standard normal distribution
 * @param heigthDist        distribution for the y-coordinate of the
 * origin(position) 0 = uniform, 1 = standard normal distribution
 * @param horDist           distribution for the x-coordinate of the direction 0
 * = uniform, 1 = standard normal distribution
 * @param verDist           distribution for the y-coordinate of the direction 0
 * = uniform, 1 = standard normal distribution
 * @param linPol0           polarization
 * @param linPol45          polarization
 * @param circPol           polarization
 * @param misalignment      if the source is moved/turned in any direction
 * (affects x,y position and x,y direction)
 *
 */
PointSource::PointSource(const std::string& name, int numberOfRays,
                         EnergyDistribution dist, const double sourceWidth,
                         const double sourceHeight, const double sourceDepth,
                         const double horDivergence, const double verDivergence,
                         SourceDist widthDist, SourceDist heightDist,
                         SourceDist horDist, SourceDist verDist,
                         const double linPol0, const double linPol45,
                         const double circPol,
                         const std::array<double, 6> misalignment)
    : LightSource(name.c_str(), std::move(dist), linPol0, linPol45, circPol, misalignment,
                  sourceDepth, sourceHeight, sourceWidth, horDivergence,
                  verDivergence),
      m_numberOfRays(numberOfRays) {
    m_widthDist = widthDist;
    m_heightDist = heightDist;
    m_horDist = horDist;
    m_verDist = verDist;
}

PointSource::~PointSource() = default;

// returns nullptr on error
std::shared_ptr<PointSource> PointSource::createFromXML(const RAYX::xml::Parser& p) {
    return std::make_shared<PointSource>(
        p.name(), p.parseNumberRays(), p.parseEnergyDistribution(),
        p.parseSourceWidth(), p.parseSourceHeight(), p.parseSourceDepth(),
        p.parseHorDiv(), p.parseVerDiv(), p.parseSourceWidthDistribution(),
        p.parseSourceHeightDistribution(), p.parseHorDivDistribution(),
        p.parseVerDivDistribution(), p.parseLinearPol0(), p.parseLinearPol45(),
        p.parseCircularPol(), p.parseMisalignment());
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
RayList PointSource::getRays() const {
    double x, y, z, psi, phi,
        en;  // x,y,z pos, psi,phi direction cosines, en=energy

    RandomState rs;

    int n = m_numberOfRays;
    RayList rayList;
    // rayList.reserve(1048576);
    RAYX_D_LOG << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    for (int i = 0; i < n; i++) {
        x = getCoord(m_widthDist, m_sourceWidth, rs) +
            getMisalignmentParams()[0];
        y = getCoord(m_heightDist, m_sourceHeight, rs) +
            getMisalignmentParams()[1];
        z = (rs.m_uniformDist(rs.m_randEngine) - 0.5) * m_sourceDepth;
        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        psi = getCoord(m_verDist, m_verDivergence, rs) +
              getMisalignmentParams()[2];
        phi = getCoord(m_horDist, m_horDivergence, rs) +
              getMisalignmentParams()[3];
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 stokes =
            glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r = {position, 1.0, direction, en, stokes, 0.0, 0.0, 0.0, 0.0};

        rayList.push(r);
    }
    // rayList.resize(1048576);
    return rayList;
}

}  // namespace RAYX
