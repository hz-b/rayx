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

double Ray::getxDir() const { return m_direction.x; }
double Ray::getyDir() const { return m_direction.y; }
double Ray::getzDir() const { return m_direction.z; }
double Ray::getxPos() const { return m_position.x; }
double Ray::getyPos() const { return m_position.y; }
double Ray::getzPos() const { return m_position.z; }
double Ray::getEnergy() const { return m_energy; }
double Ray::getWeight() const { return m_weight; }
double Ray::getS0() const { return m_stokes.x; }
double Ray::getS1() const { return m_stokes.y; }
double Ray::getS2() const { return m_stokes.z; }
double Ray::getS3() const { return m_stokes.w; }
double Ray::getPathLength() const { return m_pathLength; }
double Ray::getOrder() const { return m_order; }
double Ray::getLastElement() const { return m_lastElement; }
double Ray::getExtraParam() const { return m_extraParam; }

}  // namespace RAYX