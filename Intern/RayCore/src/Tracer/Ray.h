#pragma once
#pragma pack(16)

#include <glm.hpp>

#include "Core.h"

namespace RAYX {
struct RAYX_API Ray {
    static Ray makeRayFrom(const glm::dvec3& origin, const glm::dvec3& direction, const glm::dvec4& stokes, const double energy, const double weight) {
        Ray ray;
        ray.m_position = {origin.x, origin.y, origin.z};
        ray.m_direction = {direction.x, direction.y, direction.z};
        ray.m_stokes = {stokes.x, stokes.y, stokes.z, stokes.w};
        ray.m_energy = energy;
        ray.m_weight = weight;
        ray.m_pathLength = 0.0;
        ray.m_order = 0.0;
        ray.m_lastElement = 0.0;
        ray.m_extraParam = 0.0;
        return ray;
    }
    // TODO(Jannis): remove
    double getxDir() const { return m_direction.x; }
    double getyDir() const { return m_direction.y; }
    double getzDir() const { return m_direction.z; }
    double getxPos() const { return m_position.x; }
    double getyPos() const { return m_position.y; }
    double getzPos() const { return m_position.z; }
    double getEnergy() const { return m_energy; }
    double getWeight() const { return m_weight; }
    double getS0() const { return m_stokes.x; }
    double getS1() const { return m_stokes.y; }
    double getS2() const { return m_stokes.z; }
    double getS3() const { return m_stokes.w; }
    double getPathLength() const { return m_pathLength; }
    double getOrder() const { return m_order; }
    double getLastElement() const { return m_lastElement; }
    double getExtraParam() const { return m_extraParam; }

    struct vec3 {
        double x, y, z;
    };
    struct vec4 {
        double x, y, z, w;
    };
    vec3 m_position;
    double m_weight;
    vec3 m_direction;
    double m_energy;
    vec4 m_stokes;
    double m_pathLength;
    double m_order;
    double m_lastElement;
    double m_extraParam;
};
}  // namespace RAYX