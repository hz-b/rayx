
#include "CircleSource.h"

#include <DesignElement/DesignSource.h>
#include <Data/xml.h>
#include <Debug/Debug.h>
#include <Debug/Instrumentor.h>
#include <Random.h>
#include <Shader/Constants.h>

namespace RAYX {

CircleSource::CircleSource(const DesignSource& dSource) : LightSource(dSource) {
    m_stokes = dSource.getStokes();

    m_sourceDepth = dSource.getSourceDepth();
    m_sourceHeight = dSource.getSourceHeight();
    m_sourceWidth = dSource.getSourceWidth();

    m_misalignment = dSource.getMisalignment();

    m_numOfCircles = dSource.getNumOfCircles();
    m_maxOpeningAngle = dSource.getMaxOpeningAngle();
    m_minOpeningAngle = dSource.getMinOpeningAngle();
    m_deltaOpeningAngle = dSource.getDeltaOpeningAngle();
}
/**
 * Creates random rays from circle source with specified num. of circles and
 * spread angles
 * origins are distributed uniformly, the pattern shows on the next element
 * through the directions
 * @returns list of rays
 */
std::vector<Ray> CircleSource::getRays([[maybe_unused]] int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();
    double x, y, z, en;  // x,y,z pos, psi,phi direction cosines, en=energy

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);

    // create n rays with random position and divergence within the given span
    // for width, height, depth
    for (int i = 0; i < n; i++) {
        x = (randomDouble() - 0.5) * m_sourceWidth;
        x += m_position.x;
        y = (randomDouble() - 0.5) * m_sourceHeight;
        y += m_position.y;
        z = (randomDouble() - 0.5) * m_sourceDepth;
        z += m_position.z;

        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get corresponding direction to create circles
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirection();

        Ray r = {position, ETYPE_UNINIT, direction, en, m_stokes, 0.0, 0.0, -1.0, -1.0};

        rayList.push_back(r);
    }
    return rayList;
}

/**
 * calculate directions to form circles on the next element
 * calculations taken from RAY-UI
 */
glm::dvec3 CircleSource::getDirection() const {
    double angle = randomDouble() * 2.0 * PI;
    int circle;

    circle = randomIntInRange(1, m_numOfCircles) - 1;

    double thetabetweencircles = (m_maxOpeningAngle.rad - m_minOpeningAngle.rad) / (m_numOfCircles - 1.0);
    double theta = thetabetweencircles * circle;
    theta = theta + (randomDouble() - 0.5) * m_deltaOpeningAngle.rad + m_minOpeningAngle.rad;

    double al = cos(angle) * cos(m_misalignment.m_rotationYerror.rad);
    al = al + sin(angle) * sin(m_misalignment.m_rotationYerror.rad) * sin(m_misalignment.m_rotationXerror.rad);
    al = al * sin(theta);
    al = al + cos(m_misalignment.m_rotationXerror.rad) * cos(theta) * sin(m_misalignment.m_rotationYerror.rad);

    double am = -cos(theta) * sin(m_misalignment.m_rotationXerror.rad);
    am = am + cos(m_misalignment.m_rotationXerror.rad) * sin(angle) * sin(theta);

    double an = (-cos(angle) * sin(m_misalignment.m_rotationYerror.rad)) * sin(theta);
    an = an + cos(m_misalignment.m_rotationYerror.rad) * cos(m_misalignment.m_rotationXerror.rad) * cos(theta);
    an = an + cos(m_misalignment.m_rotationYerror.rad) * sin(angle) * sin(m_misalignment.m_rotationXerror.rad) * sin(theta);

    return glm::dvec3(al, am, an);
}


}  // namespace RAYX
