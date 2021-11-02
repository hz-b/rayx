#include "Toroid.h"
#include <cassert>
#include <math.h>

namespace RAYX
{
    /**
     * @param inputPoints           16 entry vector that contains the paramters for a toroidal surface
    */
    Toroid::Toroid(const std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16);
        m_parameters = inputPoints;
    }

    /**
     * @param longRadius            long radius of the toroidal shape
     * @param shortRadius           short radius of the toroidal shape
    */
    Toroid::Toroid(double longRadius, double shortRadius)
    {
        m_parameters = std::vector<double>{longRadius, shortRadius,0,0, 0,0,0,0, 0,0,0,0, 6,0,0,0};
    }

    Toroid::Toroid() {}

    Toroid::~Toroid()
    {
    }

    /**
     * sets the parameters of the toroidal surface to the given 16 entry vector
     * @param inputPoints   16 entry vector a_11 to a_44
     * @return void
    */
    void Toroid::setParameters(const std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16); //parameter size ==6?
        m_parameters = inputPoints;
    }

    std::vector<double> Toroid::getParams() const {
        std::cout << "[Toroid]: Return anchor points" << std::endl;
        return m_parameters;
    }

} // namespace RAYX