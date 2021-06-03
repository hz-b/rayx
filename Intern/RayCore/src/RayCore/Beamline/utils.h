#include <math.h>
#include <vector>
#include <iostream>
#include <stdexcept>
typedef std::vector<std::vector<double>> Matrix;

// useful constants
const double inm2eV = 1.239852e3;
const double PI = 3.14159265358979323;

// useful functions
double hvlam(double x);
double rad(double degree);
double degree(double rad);

Matrix getMatrixProduct(Matrix A, Matrix B);
std::vector<double> getMatrixProductAsVector(std::vector<double> A, std::vector<double> B);
Matrix getVectorAsMatrix(std::vector<double> V);
