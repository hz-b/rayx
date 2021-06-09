#include "PointSource.h"
#include <cassert>
#include <cmath>

namespace RAY
{


    PointSource::PointSource(int id, std::string name, int numberOfRays, int spreadType,
        double sourceWidth, double sourceHeight, double sourceDepth, double horDivergence,
        double verDivergence, int widthDist, int heightDist, int horDist, int verDist,
        double photonEnergy, double energySpread, std::vector<double> misalignment)
        : LightSource(id, numberOfRays, name.c_str(), spreadType, photonEnergy, energySpread, misalignment),
        m_sourceDepth(sourceDepth),
        m_sourceHeight(sourceHeight),
        m_sourceWidth(sourceWidth),
        m_horDivergence(horDivergence),
        m_verDivergence(verDivergence)
    {
        m_widthDist = widthDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_heightDist = heightDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_horDist = horDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_verDist = verDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        std::normal_distribution<double> m_stdnorm(0, 1);
        std::uniform_real_distribution<double> m_uniform(0, 1);
        std::default_random_engine m_re;

    }

    PointSource::~PointSource() {}

    /**
     * creates random rays from point source with specified width and height
     * distributed according to either uniform or gaussian distribution across width & height of source
     * the deviation of the direction of each ray from the main ray (0,0,1, phi=psi=0) can also be specified to be
     * uniform or gaussian within a given range (m_verDivergence, m_horDivergence)
     * z-position of ray is always from uniform distribution
     *
     * returns list of rays
     */
    std::vector<Ray> PointSource::getRays() {
        // double mean = 0;
        // double stddev = 1;
        // std::normal_distribution<double> stdnorm (mean,stddev); 
        // std::uniform_real_distribution<double> uniform (0,1);

        double x, y, z, psi, phi, en; //x,y,z pos, psi,phi direction cosines, en=energy

        int n = this->getNumberOfRays();
        std::vector<Ray> rayVector;
        rayVector.reserve(1048576);
        std::cout << "create " << n << " rays with standard normal deviation..." << std::endl;

        // create n rays with random position and divergence within the given span for width, height, depth, horizontal and vertical divergence
        for (int i = 0; i < n; i++) {
            x = getCoord(m_widthDist, m_sourceWidth) + getMisalignmentParams()[0];
            y = getCoord(m_heightDist, m_sourceHeight) + getMisalignmentParams()[1];
            z = (m_uniform(m_re) - 0.5) * m_sourceDepth;
            en = selectEnergy();
            //double z = (rn[2] - 0.5) * m_sourceDepth;
            glm::dvec3 position = glm::dvec3(x, y, z);

            // get random deviation from main ray based on distribution
            psi = getCoord(m_verDist, m_verDivergence) + getMisalignmentParams()[2];
            phi = getCoord(m_horDist, m_horDivergence) + getMisalignmentParams()[3];
            // get corresponding angles based on distribution and deviation from main ray (main ray: x=0,y=0,z=1 if phi=psi=0)
            glm::dvec3 direction = getDirectionFromAngles(phi, psi);
            Ray r = Ray(position, direction, en, 1.0);
            rayVector.emplace_back(r);
        }
        std::cout << &(rayVector[0]) << std::endl;
        //rayVector.resize(1048576);
        return rayVector;
    }

    /**
     * get deviation from main ray according to specified distribution (uniform if hard edge, gaussian if soft edge)) and extent (eg specified width/height of source)
     */
    double PointSource::getCoord(PointSource::SOURCE_DIST l, double extent) {
        if (l == SD_HARDEDGE) {
            return (m_uniform(m_re) - 0.5) * extent;
        }
        else {
            return (m_stdnorm(m_re) * extent);
        }
    }

    double PointSource::getSourceDepth() { return m_sourceDepth; }
    double PointSource::getSourceHeight() { return m_sourceHeight; }
    double PointSource::getSourceWidth() { return m_sourceWidth; }
    double PointSource::getVerDivergence() { return m_verDivergence; }
    double PointSource::getHorDivergence() { return m_horDivergence; }
} // namespace RAY