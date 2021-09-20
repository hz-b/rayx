#pragma once

#include <math.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Core.h"
#include "Ray.h"
#include "Beamline/OpticalElement.h"
#include "utils.h"
#include <memory>
#include <glm.hpp>

typedef struct GeometricUserParams {
    double alpha;
    double beta;
    double chi;
    double dist;
    std::vector<double> misalignment;
    GeometricUserParams(double alpha, double beta, double chi, double dist, std::vector<double> misalignment) : alpha(alpha), beta(beta), chi(chi), dist(dist), misalignment(misalignment) {}
} GeometricUserParams;

glm::dmat4x4 calcE2B(GeometricUserParams p);

glm::dvec4 calcPosition(GeometricUserParams current, GeometricUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or);
glm::dvec4 calcPosition(GeometricUserParams current);
glm::dmat4x4 calcOrientation(GeometricUserParams current, GeometricUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or);
glm::dmat4x4 calcOrientation(GeometricUserParams current);
