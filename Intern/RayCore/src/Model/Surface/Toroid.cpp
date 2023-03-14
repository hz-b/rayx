#include "Toroid.h"

#include "Constants.h"
#include "Debug/Debug.h"

namespace RAYX {

/**
 * @param longRadius            long radius of the toroidal shape
 * @param shortRadius           short radius of the toroidal shape
 * @param elementType           6=toroid mirror, 4=toroid rzp
 */
Toroid::Toroid(double longRadius, double shortRadius) {
    m_parameters = {longRadius, shortRadius, 0, 0,  //
                    0,          0,           0, 0,  //
                    0,          0,           0, 0,  //
                    0,          0,           0, 0};
}

int Toroid::getSurfaceType() const { return STY_TOROID; }

Toroid::Toroid() = default;

Toroid::~Toroid() = default;

/**
 * ENCODING:
 *
 * {longRadius,  shortRadius,   0.0,       0.0,
 *  0.0,         0.0,           0.0,       0.0,
 *  0.0,         0.0,           0.0,       0.0,
 *  type,        settings,      material,  0.0}
 *
 * @param type = what kind of optical element (mirror, plane grating, spherical
 *grating, toroid mirror, rzp, slit..)
 * @param setting = how to interpret the input of
 *these params. During normal tracing always = 0 but when testing, this
 *parameter defines which test to run
 * @param material = which material the surface has, stored as a number
 *corresponding to the variants of the enum class Material (see Material.h and
 *materials.xmacro)
 *
 **/
std::array<double, 16> Toroid::getParams() const {
    RAYX_VERB << "Return surface points";
    return m_parameters;
}

}  // namespace RAYX
