#include "PointSource.h"
#include <cassert>
#include <cmath>

namespace RAY
{
    

    PointSource::PointSource(int id, std::string name, int numberOfRays, double sourceWidth, double sourceHeight,
    double sourceDepth, double horDivergence, double verDivergence, int widthLength, int heightLength, int horLength, int verLength) : LightSource(id, numberOfRays, name.c_str()), m_sourceDepth(sourceDepth), m_sourceHeight(sourceHeight), m_sourceWidth(sourceWidth), m_horDivergence(horDivergence), m_verDivergence(verDivergence) {
        m_widthLength = widthLength == 0 ? SL_GAUSSIAN : SL_HARDEDGE;
        m_heightLength = heightLength == 0 ? SL_GAUSSIAN : SL_HARDEDGE;
        m_horLength = horLength == 0 ? SL_GAUSSIAN : SL_HARDEDGE;
        m_verLength = verLength == 0 ? SL_GAUSSIAN : SL_HARDEDGE;
        std::normal_distribution<double> m_stdnorm(0,1);
        std::uniform_real_distribution<double> m_uniform(0,1);
        std::default_random_engine m_re;
        
    }
        
    PointSource::~PointSource(){}

    /**
     * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as columns, eg amountOfRays=20 -> 4*4=16, rest (4 rays) same as first 4)
     * distributed evenly across width & height of source 
     * returns list of rays
     */
    std::vector<Ray> PointSource::getRays() {
        double mean = 0;
        double stddev = 1;
        // std::normal_distribution<double> stdnorm (mean,stddev); 
        // std::uniform_real_distribution<double> uniform (0,1);
        
        double x,y,z,psi,phi;
        
        int n = this->getNumberOfRays();
        std::vector<Ray> rayVector;
        rayVector.reserve(1048576);
        std::cout << "create " << n << " rays with standard normal deviation..." << std::endl;
        // create n rays with random position and divergence within the given span for width, height, ..
        for(int i = 0; i<n; i++) {
            x = getCoord(m_widthLength, m_sourceWidth);
            y = getCoord(m_heightLength, m_sourceHeight);
            z = (m_uniform(m_re) - 0.5) * m_sourceDepth;
            //double z = (rn[2] - 0.5) * m_sourceDepth;
            glm::dvec3 position = glm::dvec3(x, y, z);
            
            phi = getCoord(m_horLength, m_horDivergence);
            psi = getCoord(m_verLength, m_verDivergence);
            //std::cout << phi <<" "<<psi << std::endl;
            // double phi = (rn[3] - 0.5) * m_horDivergence;
            // double psi = (rn[4] - 0.5) * m_verDivergence;
            glm::dvec3 direction = getDirectionFromAngles(phi,psi);
            //std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
            Ray r = Ray(position, direction, 1.0);
            rayVector.emplace_back(r);
        }
        std::cout<<&(rayVector[0])<<std::endl;
        rayVector.resize(1048576);
        return rayVector;
    }

    double PointSource::getCoord(PointSource::SOURCE_LENGTH l, double extent) {
        if(l == SL_HARDEDGE)
            return (m_uniform(m_re) - 0.5) * extent;
        else
            return (m_stdnorm(m_re));

    }

    double PointSource::getSourceDepth(){ return m_sourceDepth; }
    double PointSource::getSourceHeight(){ return m_sourceHeight; }
    double PointSource::getSourceWidth(){ return m_sourceWidth; }
    double PointSource::getVerDivergence(){ return m_verDivergence; }
    double PointSource::getHorDivergence(){ return m_horDivergence; }
} // namespace RAY