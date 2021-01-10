#include "Ray.h"

#include <iostream>

namespace RAY
{

    Ray::Ray(glm::dvec3 position, glm::dvec3 direction, double weight) : m_position(position),
                                                                       m_direction(direction), m_weight(weight)
    {
    }

    Ray::~Ray()
    {
    }

} // namespace RAY