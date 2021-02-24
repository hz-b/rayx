#include "Ray.h"

#include <iostream>

namespace RAY
{

    Ray::Ray(glm::dvec3 position, glm::dvec3 direction, double weight) : m_position(position),
                                                                       m_direction(direction), m_weight(weight), m_placeholder(0)
    {
    }
    Ray::Ray()
    {
        m_position = {0,0,0};
        m_direction = {0,0,0};
        m_weight = 0;
        m_placeholder = 0;
    }

    Ray::~Ray()
    {
    }

} // namespace RAY