#pragma once

#include <math.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Core.h"
#include "Ray.h"
#include <memory>
#include <glm.hpp>

typedef std::vector<std::vector<double>> Matrix;

// useful constants
const double inm2eV = 1.239852e3;
const double PI = 3.14159265358979323;

// useful functions
double RAYX_API hvlam(double x);
double RAYX_API degToRad(double degree);
double RAYX_API radToDeg(double rad);

Matrix RAYX_API getMatrixProduct(Matrix A, Matrix B);
std::vector<double> RAYX_API getMatrixProductAsVector(std::vector<double> A, std::vector<double> B);
Matrix RAYX_API getVectorAsMatrix(std::vector<double> V);

void printDMat4(glm::dmat4 matrix);
void printMatrix(std::vector<double> matrix);

std::vector<double> glmToVector16(glm::dmat4x4 m);
glm::dmat4x4 vectorToGlm16(std::vector<double> m);