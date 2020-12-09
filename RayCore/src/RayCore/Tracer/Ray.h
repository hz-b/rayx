#pragma once

#include "Core.h"

#include <glm.hpp>

namespace RAY
{
    class RAY_API Ray
    {
    public:
        Ray(glm::vec3 position, glm::vec3 direction, double weight);
        ~Ray();

    private:
        glm::vec3 &m_position;
        glm::vec3 &m_direction;
        double &m_weight;
    };
} // namespace RAY