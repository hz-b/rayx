#include "PointSource.h"

#include <Data/xml.h>

#include <cmath>

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
PointSource::PointSource(const std::string name, EnergyDistribution dist,
                         const double sourceWidth, const double sourceHeight,
                         const double sourceDepth, const double horDivergence,
                         const double verDivergence, const int widthDist,
                         const int heightDist, const int horDist,
                         const int verDist, const double linPol0,
                         const double linPol45, const double circPol,
                         const std::array<double, 6> misalignment)
    : LightSource(name.c_str(), dist, linPol0, linPol45, circPol, misalignment,
                  sourceDepth, sourceHeight, sourceWidth, horDivergence,
                  verDivergence) {
    m_widthDist = widthDist == 0 ? SD_UNIFORM : SD_GAUSSIAN;
    m_heightDist = heightDist == 0 ? SD_UNIFORM : SD_GAUSSIAN;
    m_horDist = horDist == 0 ? SD_UNIFORM : SD_GAUSSIAN;
    m_verDist = verDist == 0 ? SD_UNIFORM : SD_GAUSSIAN;
    std::normal_distribution<double> m_stdnorm(0, 1);
    std::uniform_real_distribution<double> m_uniform(0, 1);
    std::default_random_engine m_re;
}

PointSource::~PointSource() {}

// returns nullptr on error
std::shared_ptr<PointSource> PointSource::createFromXML(
    rapidxml::xml_node<>* node) {
    const std::string name = node->first_attribute("name")->value();

    if (!xml::paramInt(node, "numberRays", &SimulationEnv::get().m_numOfRays)) {
        return nullptr;
    }

    EnergyDistribution energyDistribution;
    if (!xml::paramEnergyDistribution(node, &energyDistribution)) {
        return nullptr;
    }

    double sourceWidth;
    if (!xml::paramDouble(node, "sourceWidth", &sourceWidth)) {
        return nullptr;
    }

    double sourceHeight;
    if (!xml::paramDouble(node, "sourceHeight", &sourceHeight)) {
        return nullptr;
    }

    double sourceDepth;
    if (!xml::paramDouble(node, "sourceDepth", &sourceDepth)) {
        return nullptr;
    }

    double horDivergence;
    if (!xml::paramDouble(node, "horDiv", &horDivergence)) {
        return nullptr;
    }

    double verDivergence;
    if (!xml::paramDouble(node, "verDiv", &verDivergence)) {
        return nullptr;
    }

    int widthDist;
    if (!xml::paramInt(node, "sourceWidthDistribution", &widthDist)) {
        return nullptr;
    }

    int heightDist;
    if (!xml::paramInt(node, "sourceHeightDistribution", &heightDist)) {
        return nullptr;
    }

    int horDist;
    if (!xml::paramInt(node, "horDivDistribution", &horDist)) {
        return nullptr;
    }

    int verDist;
    if (!xml::paramInt(node, "verDivDistribution", &verDist)) {
        return nullptr;
    }

    double linPol0;
    if (!xml::paramDouble(node, "linearPol_0", &linPol0)) {
        return nullptr;
    }

    double linPol45;
    if (!xml::paramDouble(node, "linearPol_45", &linPol45)) {
        return nullptr;
    }

    double circPol;
    if (!xml::paramDouble(node, "circularPol", &circPol)) {
        return nullptr;
    }

    std::array<double, 6> misalignment;
    if (!xml::paramMisalignment(node, &misalignment)) {
        return nullptr;
    }

    return std::make_shared<PointSource>(
        name, energyDistribution, sourceWidth, sourceHeight, sourceDepth,
        horDivergence / 1000.0, verDivergence / 1000.0, widthDist, heightDist,
        horDist, verDist, linPol0, linPol45, circPol, misalignment);
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
std::vector<Ray> PointSource::getRays() {
    double x, y, z, psi, phi,
        en;  // x,y,z pos, psi,phi direction cosines, en=energy

    int n = SimulationEnv::get().m_numOfRays;
    std::vector<Ray> rayVector;
    rayVector.reserve(1048576);
    RAYX_LOG << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    for (int i = 0; i < n; i++) {
        x = getCoord(m_widthDist, m_sourceWidth) + getMisalignmentParams()[0];
        y = getCoord(m_heightDist, m_sourceHeight) + getMisalignmentParams()[1];
        z = (m_uniformDist(m_randEngine) - 0.5) * m_sourceDepth;
        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        psi = getCoord(m_verDist, m_verDivergence) + getMisalignmentParams()[2];
        phi = getCoord(m_horDist, m_horDivergence) + getMisalignmentParams()[3];
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 stokes =
            glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r = {position.x,  position.y,  position.z,  1.0,
                 direction.x, direction.y, direction.z, en,
                 stokes.x,    stokes.y,    stokes.z,    stokes.w,
                 0.0,         0.0,         0.0,         0.0};

        rayVector.emplace_back(r);
    }
    RAYX_LOG << "&rayVector: " << &(rayVector[0]);
    // rayVector.resize(1048576);
    return rayVector;
}

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
double PointSource::getCoord(const PointSource::SOURCE_DIST l,
                             const double extent) {
    if (l == SD_UNIFORM) {
        return (m_uniformDist(m_randEngine) - 0.5) * extent;
    } else {
        return (m_normDist(m_randEngine) * extent);
    }
}
}  // namespace RAYX
