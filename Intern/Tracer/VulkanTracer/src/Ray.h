#pragma once
#pragma pack(16)

#include <iostream>
#include <stdexcept>
#include <vector>

class Ray {
  public:
    Ray(double xpos, double ypos, double zpos, double xdir, double ydir,
        double zdir, double s0, double s1, double s2, double s3, double en,
        double w, double pathLength = 0, double order = 0,
        double lastElement = 0, double extraParameter = 0);
    Ray(double* location);
    Ray();
    std::vector<double> getRayInformation() const;
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

  private:
    struct vec3 {
        double x, y, z;
    };
    struct vec4 {
        double s0, s1, s2, s3;
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
