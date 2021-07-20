#pragma once

#include <math.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Core.h"
typedef std::vector<std::vector<double>> Matrix;

// useful constants
const double inm2eV = 1.239852e3;
const double PI = 3.14159265358979323;

// useful functions
double RAYX_API hvlam(double x);
double RAYX_API rad(double degree);
double RAYX_API degree(double rad);

Matrix RAYX_API getMatrixProduct(Matrix A, Matrix B);
std::vector<double> RAYX_API getMatrixProductAsVector(std::vector<double> A, std::vector<double> B);
Matrix RAYX_API getVectorAsMatrix(std::vector<double> V);
