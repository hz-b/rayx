#include "Ray.h"

#include <iostream>

namespace RAY
{

    Ray::Ray(glm::vec3 position, glm::vec3 direction, double weight) : m_position(position),
                                                        m_direction(direction), m_weight(weight)
    {
        //std::cout << "Ray created." << std::endl;
    }

    Ray::~Ray()
    {
    }

} // namespace RAY