#pragma once

#include "Core.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

namespace RAYX {

// **********************************************************
// Function to calculate the local normal incidence angle theta
// from the direction cosines of the surface normal and the ray.
// The **negative** normal vector is used.
// Input:
//   al, am, an - direction cosines of the surface normal
//   fx, fy, fz - direction cosines of the incoming ray
// Output:
//   returns theta - the angle of incidence in degrees
// **********************************************************

RAYX_FN_ACC double getTheta(Ray r, glm::dvec3 normal) {


    double al = normal[0];
    double am = normal[1];
    double an = normal[2];
    
    double fx = r.m_direction[0];
    double fy = r.m_direction[0];
    double fz = r.m_direction[2];
    
    // Normalize incoming ray vector
    double fn = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fn == 0.0) fn = 1.0;
    fx /= fn;
    fy /= fn;
    fz /= fn;

    // Dot product of normalized ray and surface normal
    double ar = fx * al + fy * am + fz * an;
    ar = std::clamp(ar, -1.0, 1.0);

    // Calculate incidence angle (in degrees)
    double theta = PI - std::acos(ar);
    theta = theta * 180.0 / PI;

    if (theta > 90.0) theta = 180.0 - theta;

    return theta;
}


RAYX_FN_ACC Ray darwin(Ray r, double theta, CrystalBehaviour b){
    
}

}