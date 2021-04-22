#pragma once
#pragma pack(16)

#include "Core.h"

#include <glm.hpp>

#define RAYCORE_RAY_DOUBLE_AMOUNT 8

namespace RAY
{
    class RAY_API Ray
    {
    public:
        // ctor
        Ray(glm::dvec3 position, glm::dvec3 direction, double weight);
        // default ctor
        Ray();
        // dtor
        ~Ray();

        glm::dvec3 m_position;
        glm::dvec3 m_direction;
        double m_weight;
        double m_placeholder;

    private:
    };
} // namespace RAY