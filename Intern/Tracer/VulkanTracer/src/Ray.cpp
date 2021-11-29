#include "Ray.h"

#include <cstring>

/**
 * initializes Ray with given position, direction, weight, energy and stokes
 * parameters. Pathlength, order and last element are set to 0
 * @param xpos
 * @param ypos
 * @param zpos
 * @param xdir
 * @param ydir
 * @param zdir
 * @param s0
 * @param s1
 * @param s2
 */
Ray::Ray(double xpos, double ypos, double zpos, double xdir, double ydir,
         double zdir, double s0, double s1, double s2, double s3, double en,
         double w, double pathLength, double order, double lastElement,
         double extraParameter) {
    m_position.x = xpos;
    m_position.y = ypos;
    m_position.z = zpos;
    m_direction.x = xdir;
    m_direction.y = ydir;
    m_direction.z = zdir;
    m_stokes.s0 = s0;
    m_stokes.s1 = s1;
    m_stokes.s2 = s2;
    m_stokes.s3 = s3;
    m_weight = w;
    m_energy = en;
    m_pathLength = pathLength;
    m_order = order;
    m_lastElement = lastElement;
    m_extraParam = extraParameter;
}
// copies 64 byts of data in the following format: xloc, yloc, zloc, weight,
// xdir, ydir, zdir, placeholder
Ray::Ray(double* location) { memcpy(&m_position.x, location, 64); }
Ray::Ray() {}

std::vector<double> Ray::getRayInformation() const{
    std::vector<double> rayInfo;
    rayInfo.push_back(m_position.x);
    rayInfo.push_back(m_position.y);
    rayInfo.push_back(m_position.z);
    rayInfo.push_back(m_direction.x);
    rayInfo.push_back(m_direction.y);
    rayInfo.push_back(m_direction.z);
    return rayInfo;
}

double Ray::getxDir() const { return m_direction.x; }
double Ray::getyDir() const { return m_direction.y; }
double Ray::getzDir() const { return m_direction.z; }
double Ray::getxPos() const { return m_position.x; }
double Ray::getyPos() const { return m_position.y; }
double Ray::getzPos() const { return m_position.z; }
double Ray::getEnergy() const { return m_energy; }
double Ray::getWeight() const { return m_weight; }
double Ray::getS0() const { return m_stokes.s0; }
double Ray::getS1() const { return m_stokes.s1; }
double Ray::getS2() const { return m_stokes.s2; }
double Ray::getS3() const { return m_stokes.s3; }
double Ray::getPathLength() const { return m_pathLength; }
double Ray::getOrder() const { return m_order; }
double Ray::getLastElement() const { return m_lastElement; }
double Ray::getExtraParam() const { return m_extraParam; }