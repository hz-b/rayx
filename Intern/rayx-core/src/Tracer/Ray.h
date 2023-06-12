#pragma once
#pragma pack(16)

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>

#include "Core.h"

namespace RAYX {
struct RAYX_API Ray {
    glm::dvec3 m_position = glm::dvec3(0, 0, 0);
    double m_weight = 0;
    glm::dvec3 m_direction = glm::dvec3(0, 0, 0);
    double m_energy = 0;
    glm::dvec4 m_stokes = glm::dvec4(0, 0, 0, 0);
    double m_pathLength = 0;
    double m_order = 0;
    double m_lastElement = 0;
    double m_extraParam = 0;

    static Ray makeRayFrom(const glm::dvec3& origin, const glm::dvec3& direction, const glm::dvec4& stokes, const double energy,
                           const double weight) {
        Ray ray;
        ray.m_position = origin;
        ray.m_direction = direction;
        ray.m_stokes = stokes;
        ray.m_energy = energy;
        ray.m_weight = weight;
        ray.m_pathLength = 0.0;
        ray.m_order = 0.0;
        ray.m_lastElement = 0.0;
        ray.m_extraParam = 0.0;
        return ray;
    }

    static Ray makeRayFrom(const glm::dvec3& origin, const glm::dvec3& direction, const glm::dvec4& stokes, const double energy, const double weight,
                           const double pathLength, const double order, const double lastElement, const double extraParam) {
        Ray ray;
        ray.m_position = origin;
        ray.m_direction = direction;
        ray.m_stokes = stokes;
        ray.m_energy = energy;
        ray.m_weight = weight;
        ray.m_pathLength = pathLength;
        ray.m_order = order;
        ray.m_lastElement = lastElement;
        ray.m_extraParam = extraParam;
        return ray;
    }

    // ! This needs to be changed everytime the order or the number of elements in the struct changes
    static Ray makeRayFromDoubles(const std::vector<double>& doubles) {
        if (doubles.size() != 16) { 
            throw std::invalid_argument("Expected 15 values in doubles");
        }
        glm::dvec3 m_position(doubles[0], doubles[1], doubles[2]);
        double m_weight = doubles[3];
        glm::dvec3 m_direction(doubles[4], doubles[5], doubles[6]);
        double m_energy = doubles[7];
        glm::dvec4 m_stokes(doubles[8], doubles[9], doubles[10], doubles[11]);
        double m_pathLength = doubles[12];
        double m_order = doubles[13];
        double m_lastElement = doubles[14];
        double m_extraParam = doubles[15];
        
        return makeRayFrom(m_position, m_direction, m_stokes, m_energy, m_weight,
                           m_pathLength, m_order, m_lastElement, m_extraParam);
    }
};
}  // namespace RAYX
