#include "Ray.h"

#include <iostream>

namespace RAY
{

    Ray::Ray(glm::dvec3 position, glm::dvec3 direction, double energy, double weight) : m_position(position),
        m_direction(direction), m_weight(weight), m_energy(energy)
    {
    }
    Ray::Ray()
    {
        m_position = { 0,0,0 };
        m_direction = { 0,0,0 };
        m_weight = 0;
        m_energy = 0;
    }

    Ray::~Ray()
    {
    }

} // namespace RAY