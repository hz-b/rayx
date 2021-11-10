#include "Ray.h"

#include <iostream>

namespace RAYX {
/**
 * @param position          position/origin of ray
 * @param direction         direction of ray
 * @param stokes            stokes parameters, needed for efficiency
 * @param energy            energy of ray (related to wavelength)
 * @param weight            initialized with 1, changed during tracing, if e.g.
 * the ray was absorbed (set to 0)
 * @param pathLength        initialized with 0, how far the ray has travelled
 * during tracing
 * @param order             order of diffraction in which the ray was reflected
 * in the last element (0 for mirrors, >0 for gratings/rzp)
 * @param lastElement       the id of the last element the ray hit during the
 * tracing process, initialized with ß
 * @param extraParam        unused parameter, only to keep the amount of doubles
 * of a ray as a multiple of 4
 */
Ray::Ray(glm::dvec3 position, glm::dvec3 direction, glm::dvec4 stokes,
         double energy, double weight, double pathLength, double order,
         double lastElement, double extraParameter)
    : m_position(position),
      m_weight(weight),
      m_direction(direction),
      m_energy(energy),
      m_stokes(stokes),
      m_pathLength(pathLength),
      m_order(order),
      m_lastElement(lastElement),
      m_extraParam(extraParameter) {}

Ray::~Ray() {}

}  // namespace RAYX