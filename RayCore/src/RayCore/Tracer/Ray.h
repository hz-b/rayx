#pragma once

#include "Core.h"

#include <glm.hpp>

#define RAYCORE_RAY_DOUBLE_AMOUNT 8

namespace RAY
{
    class RAY_API Ray
    {
    public:
        Ray(glm::dvec3 position, glm::dvec3 direction, double weight);
        Ray();
        ~Ray();
        glm::dvec3 m_position;
        double m_weight;
        glm::dvec3 m_direction;
        double m_placeholder;

    private:
    };
} // namespace RAY