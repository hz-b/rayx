#include "Quadric.h"

#include "Constants.h"
#include "Debug/Debug.h"

namespace RAYX {
/** for quadric surfaces: 16 parameters a_11,a_12,a_13,a_14,
 * a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44!
 * a_21,a_31,a_32,a_41,a_42,a_43 are never used for quadric surfaces because the
 * matrix is symmetrial, can be used for other values
 * @param inputPoints      4x4 Matrix as vector
 */
Quadric::Quadric(const std::array<double, 16> inputPoints) { m_parameters = inputPoints; }

int Quadric::getSurfaceType() const { return STY_QUADRIC; }

Quadric::Quadric() = default;

Quadric::~Quadric() = default;

/**
 * set a new set of parameters a_11 to a_44 for the quadric function
 * order: a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34,
 * a_41,a_42,a_43,a_44
 * @param inputPoints   16 entry vector a_11 to a_44
 * @return void
 */
[[maybe_unused]] void Quadric::setAnchorPoints(std::array<double, 16> inputPoints) { m_parameters = inputPoints; }

/**
 * ENCODING:
 *
 * {a_11,    a_12,     a_13,      a_14,
 *  0.0,     a_22,     a_23,      a_24,
 *  0.0,     0.0,      a_33,      a_34,
 *  icurv,   0.0,      0.0,       a_44}
 *
 * @param icurv = whether to take the first or second intersection of a ray with
 *a quadric surface
 * @param a_11, .., a_44 parameters of the quadric equation to find the
 *intersection point. Depend on the element (plane, sphere, ellipsoid,..)
 **/
std::array<double, 16> Quadric::getParams() const { return m_parameters; }

}  // namespace RAYX
