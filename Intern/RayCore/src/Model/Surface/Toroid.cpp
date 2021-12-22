#include "Toroid.h"

#include "Debug.h"
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
 * @param elementType           6=toroid mirror, 4=toroid rzp
 */
Toroid::Toroid(double longRadius, double shortRadius, double elementType) {
    m_parameters =
        std::vector<double>{longRadius, shortRadius, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0,          elementType, 0, 0, 0};
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

/**
 * ENCODING:
 *
 * {longRadius,  shortRadius,   0.0 , 0.0,
 *  0.0,         0.0,           0.0,  0.0,
 *  0.0,         0.0,           0.0,  0.0,
 *  type,        settings,      0.0,  0.0}
 *
 * @param type = what kind of optical element (mirror, plane grating, spherical
 *grating, toroid mirror, rzp, slit..)
 * @param setting = how to interpret the input of
 *these params. During normal tracing always = 0 but when testing, this
 *parameter defines which test to run
 *
 **/
std::vector<double> Toroid::getParams() const {
    RAYX_LOG << "Return surface points";
    return m_parameters;
}

}  // namespace RAYX