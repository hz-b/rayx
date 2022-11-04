#pragma once

#include <math.h>

#include <chrono>
#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"

// useful constants
const double inm2eV = 1.239852e3;
const double PI = 3.14159265358979323;

// useful functions
double RAYX_API hvlam(double x);

struct Rad;

// an angle in degrees.
struct Deg {
    Deg() = default;
    Deg(double d) : deg(d) {}

    Rad toRad() const;

    double deg;
};

// an angle in radians
struct Rad {
    Rad() = default;
    Rad(double r) : rad(r) {}

    Deg toDeg() const;
    double sin() const;
    double cos() const;
    double tan() const;

    double rad;
};

glm::dmat4x4 getRotationMatrix(double dpsi, double dphi, double dchi);

void printDVec4(glm::dvec4 vec);
void RAYX_API printDMat4(glm::dmat4 matrix);
void printDMatrix(std::array<double, 4 * 4> matrix);
bool RAYX_API isIdentMatrix(glm::dmat4x4 matrix);

std::array<double, 4 * 4> glmToArray16(glm::dmat4x4 m);
glm::dmat4x4 RAYX_API arrayToGlm16(std::array<double, 4 * 4> m);

std::array<double, 4> glmToArray4(glm::dvec4 v);
glm::dvec4 RAYX_API arrayToGlm4(std::array<double, 4> v);

std::vector<double>::iterator movingAppend(std::vector<double>&& srcVector, std::vector<double>& destVector);
