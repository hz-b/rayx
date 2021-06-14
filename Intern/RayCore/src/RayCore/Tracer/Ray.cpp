#include "Ray.h"

#include <iostream>

namespace RAY
{
    Ray::Ray(glm::dvec3 position, glm::dvec3 direction,
        double energy, double weight)
        : m_position(position),
        m_weight(weight),
        m_direction(direction),
        m_energy(energy)
    {
    }

    Ray::~Ray()
    {
    }

} // namespace RAY