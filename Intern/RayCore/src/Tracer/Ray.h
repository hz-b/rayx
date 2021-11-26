#pragma once
#pragma pack(16)

#include <glm.hpp>

#include "Core.h"

namespace RAYX {
class RAYX_API Ray {
  public:
    // ctor
    Ray(glm::dvec3 position, glm::dvec3 direction, glm::dvec4 stokes,
        double energy, double weight, double pathLength = 0, double order = 0,
        double lastElement = 0, double extraParameter = 0);
    // default ctor
    Ray() = default;
    // dtor
    ~Ray();

    glm::dvec3 m_position;
    double m_weight;
    glm::dvec3 m_direction;
    double m_energy;
    glm::dvec4 m_stokes;
    double m_pathLength;
    double m_order;
    double m_lastElement;
    double m_extraParam;

  private:
};
}  // namespace RAYX