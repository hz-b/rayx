#pragma once
#pragma pack(16)

#include <glm.hpp>

#include "Core.h"

namespace RAYX {
class RAYX_API Ray {
  public:
    // ctor
    Ray(double xpos, double ypos, double zpos, double weight, double xdir,
        double ydir, double zdir, double en, double s0, double s1, double s2,
        double s3, double pathLength = 0, double order = 0,
        double lastElement = 0, double extraParameter = 0);
    Ray(glm::dvec3 pos, glm::dvec3 dir, glm::dvec4 stokes, double energy,
        double weight, double pathLength = 0, double order = 0,
        double lastElement = 0, double extraParameter = 0);
    Ray(double* location);
    // default ctor
    Ray();
    // dtor
    ~Ray();

    double getxDir() const;
    double getyDir() const;
    double getzDir() const;
    double getxPos() const;
    double getyPos() const;
    double getzPos() const;
    double getEnergy() const;
    double getWeight() const;
    double getS0() const;
    double getS1() const;
    double getS2() const;
    double getS3() const;
    double getPathLength() const;
    double getOrder() const;
    double getLastElement() const;
    double getExtraParam() const;

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