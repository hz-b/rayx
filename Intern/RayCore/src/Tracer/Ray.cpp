#include "Ray.h"

#include <iostream>

namespace RAYX
{
    Ray::Ray(glm::dvec3 position, glm::dvec3 direction, glm::dvec4 stokes,
        double energy, double weight, double pathLength, double order, double lastElement, double extraParameter)
        : m_position(position),
        m_weight(weight),
        m_direction(direction),
        m_energy(energy),
        m_stokes(stokes),
        m_pathLength(pathLength),
        m_order(order),
        m_lastElement(lastElement),
        m_extraParam(extraParameter)
    {
    }

    Ray::~Ray()
    {
    }

} // namespace RAYX