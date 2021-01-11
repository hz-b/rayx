#include "MatrixSource.h"
#include <cassert>
#include <cmath>

namespace RAY
{

    // angles given and stored in rad
    MatrixSource::MatrixSource(int id, std::string name, int numberOfRays, double sourceWidth, double sourceHeight, double sourceDepth, double horDivergence, double verDivergence) 
    : LightSource(id, numberOfRays, name.c_str()), m_sourceDepth(sourceDepth), m_sourceHeight(sourceHeight), m_sourceWidth(sourceWidth), m_horDivergence(horDivergence), m_verDivergence(verDivergence) {}

    MatrixSource::~MatrixSource()
    {
    }

    /**
     * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as columns, eg amountOfRays=20 -> 4*4=16 rays)
     * distributed evenly across width & height of source 
     * returns list of rays
     */
    std::vector<Ray *> MatrixSource::getRays() {
        int rmat1 = int(sqrt(this->getNumberOfRays()));
        std::vector<Ray *> rayList;
        std::cout << "create " << rmat1 << " times " << rmat1 << " matrix with Matrix Source..." << std::endl;
        // fill the square with rmat1xrmat1 rays
        for(int row = 0; row<rmat1; row++) {
            for(int col = 0; col<rmat1; col++) {
                double rn = 0.5; // random in [0,1]
                double x = -0.5*m_sourceWidth + (m_sourceWidth/(rmat1-1)) * row;
                double y = -0.5*m_sourceHeight + (m_sourceHeight/(rmat1-1)) * col;
                double z = (rn - 0.5) * m_sourceDepth;
                glm::dvec3 position = glm::dvec3(x, y, z);
                
                double phi = -0.5*m_horDivergence + (m_horDivergence/(rmat1-1)) * row;
                double psi = -0.5*m_verDivergence + (m_verDivergence/(rmat1-1)) * col;
                glm::dvec3 direction = getDirectionFromAngles(phi,psi);

                Ray* r = new Ray(position, direction, 1.0);
                rayList.emplace_back(r);
            }
        }
        // afterwards start from the beginning again
        for(int i = 0; i<this->getNumberOfRays()-rmat1*rmat1; i++) {
            rayList.emplace_back(rayList.at(i));
        }
        return rayList;
    }

    double MatrixSource::getSourceDepth(){ return m_sourceDepth; }
    double MatrixSource::getSourceHeight(){ return m_sourceHeight; }
    double MatrixSource::getSourceWidth(){ return m_sourceWidth; }
    double MatrixSource::getVerDivergence(){ return m_verDivergence; }
    double MatrixSource::getHorDivergence(){ return m_horDivergence; }
    
} // namespace RAY