#include "Ray.h"

#include <iostream>

namespace RAYX
{
    Ray::Ray(glm::dvec3 position, glm::dvec3 direction, glm::dvec4 stokes,
        double energy, double weight)
        : m_position(position),
        m_weight(weight),
        m_direction(direction),
        m_energy(energy),
        m_stokes(stokes)
    {
    }

    Ray::~Ray()
    {
    }

} // namespace RAYX