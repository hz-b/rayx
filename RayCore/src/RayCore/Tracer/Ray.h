#pragma once

#include <glm.hpp>

namespace RAY
{
    class Ray
    {
    public:
        Ray(glm::vec3 position, glm::vec3 direction);
        ~Ray();

    private:
        glm::vec3 &m_position;
        glm::vec3 &m_direction;
    };
} // namespace RAY