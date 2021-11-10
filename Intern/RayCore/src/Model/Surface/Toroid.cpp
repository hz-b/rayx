#include "Toroid.h"

#include <math.h>

#include <cassert>

namespace RAYX {
/**
 * @param inputPoints           16 entry vector that contains the parameters for
 * a toroidal surface
 */
Toroid::Toroid(const std::vector<double> inputPoints) {
    assert(inputPoints.size() == 16);
    m_parameters = inputPoints;
}

/**
 * @param longRadius            long radius of the toroidal shape
 * @param shortRadius           short radius of the toroidal shape
 */
Toroid::Toroid(double longRadius, double shortRadius) {
    m_parameters = std::vector<double>{
        longRadius, shortRadius, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0};
    m_longRadius = longRadius;
    m_shortRadius = shortRadius;
}

Toroid::Toroid() {}

Toroid::~Toroid() {}

/**
 * sets the parameters of the toroidal surface to the given 16 entry vector
 * @param inputPoints   16 entry vector
 * @return void
 */
void Toroid::setParameters(const std::vector<double> inputPoints) {
    assert(inputPoints.size() == 16);
    m_parameters = inputPoints;
    m_longRadius = inputPoints[0];
    m_shortRadius = inputPoints[1];
}

std::vector<double> Toroid::getParams() const {
    std::cout << "[Toroid]: Return surface points" << std::endl;
    return m_parameters;
}

}  // namespace RAYX