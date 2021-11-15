#pragma once

#include <math.h>

#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"
#include "Ray.h"

typedef std::vector<std::vector<double>> Matrix;

// useful constants
const double inm2eV = 1.239852e3;
const double PI = 3.14159265358979323;

// useful functions
double RAYX_API hvlam(double x);
double RAYX_API degToRad(double degree);
double RAYX_API radToDeg(double rad);

glm::dmat4x4 getRotationMatrix(double dpsi, double dphi, double dchi);

void printDMat4(glm::dmat4 matrix);
void printMatrix(std::vector<double> matrix);

std::vector<double> glmToVector16(glm::dmat4x4 m);
glm::dmat4x4 vectorToGlm16(std::vector<double> m);
std::vector<double> glmToVector4(glm::dvec4 v);