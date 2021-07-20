#include "MatrixSource.h"
#include <cassert>
#include <random>

namespace RAYX
{

    // angles given and stored in rad
    MatrixSource::MatrixSource(const int id, const std::string name, const int numberOfRays, const int spreadType, const double sourceWidth,
        const double sourceHeight, const double sourceDepth, const double horDivergence, const double verDivergence,
        const double photonEnergy, const double energySpread, const std::vector<double> misalignment)
        : LightSource(id, numberOfRays, name.c_str(), spreadType, photonEnergy, energySpread, misalignment),
        m_sourceDepth(sourceDepth),
        m_sourceHeight(sourceHeight),
        m_sourceWidth(sourceWidth),
        m_horDivergence(horDivergence),
        m_verDivergence(verDivergence)
    {

    }

    MatrixSource::~MatrixSource()
    {
    }

    /**
     * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as columns, eg amountOfRays=20 -> 4*4=16, rest (4 rays) same as first 4)
     * distributed evenly across width & height of source
     * returns list of rays
     */
    std::vector<Ray> MatrixSource::getRays() {
        double lower_bound = 0;
        double upper_bound = 1;
        std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
        std::default_random_engine re;

        double x, y, z, psi, phi, en; //x,y,z pos, psi,phi direction cosines, en=energy
        int rmat = int(sqrt(this->getNumberOfRays()));

        std::vector<Ray> rayVector;
        rayVector.reserve(1048576);
        std::cout << "create " << rmat << " times " << rmat << " matrix with Matrix Source..." << std::endl;
        // fill the square with rmat1xrmat1 rays
        for (int col = 0; col < rmat; col++) {
            for (int row = 0; row < rmat; row++) {
                double rn = unif(re); // uniform random in [0,1)
                x = -0.5 * m_sourceWidth + (m_sourceWidth / (rmat - 1)) * row + getMisalignmentParams()[0];;
                y = -0.5 * m_sourceHeight + (m_sourceHeight / (rmat - 1)) * col + getMisalignmentParams()[1];;
                z = (rn - 0.5) * m_sourceDepth;
                en = selectEnergy();
                glm::dvec3 position = glm::dvec3(x, y, z);

                phi = -0.5 * m_horDivergence + (m_horDivergence / (rmat - 1)) * row + getMisalignmentParams()[2];;
                psi = -0.5 * m_verDivergence + (m_verDivergence / (rmat - 1)) * col + getMisalignmentParams()[3];;
                glm::dvec3 direction = getDirectionFromAngles(phi, psi);

                Ray r = Ray(position, direction, en, 1.0);
                rayVector.push_back(r);
            }
        }
        // afterwards start from the beginning again
        for (int i = 0; i < this->getNumberOfRays() - rmat * rmat; i++) {
            Ray r = rayVector.at(i);
            glm::dvec3 position = glm::dvec3(r.m_position[0], r.m_position[1], r.m_position[2]);
            glm::dvec3 direction = glm::dvec3(r.m_direction[0], r.m_direction[1], r.m_direction[2]);
            en = selectEnergy();
            Ray r_copy(position, direction, en, 1.0);
            rayVector.push_back(r_copy);
        }
        std::cout << &(rayVector[0]) << std::endl;
        //rayVector.resize(1048576);
        return rayVector;
    }

} // namespace RAYX