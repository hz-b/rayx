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
        ~Ray();
        glm::dvec3 m_position;
        glm::dvec3 m_direction;
        double m_weight;

    private:
    };
} // namespace RAY