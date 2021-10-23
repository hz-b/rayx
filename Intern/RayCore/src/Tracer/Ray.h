#pragma once
#pragma pack(16)

#include "Core.h"

#include <glm.hpp>

#define RAYCORE_RAY_DOUBLE_AMOUNT 8

namespace RAYX
{
    class RAYX_API Ray
    {
    public:
        // ctor
        Ray(glm::dvec3 position, glm::dvec3 direction, glm::dvec4 stokes, double energy, double weight);
        // default ctor
        Ray() = default;
        // dtor
        ~Ray();

        glm::dvec3 m_position;
        double m_weight;
        glm::dvec3 m_direction;
        double m_energy;
        glm::dvec4 m_stokes;

    private:
    };
} // namespace RAYX