#pragma once

#include <math.h>

#include <array>
#include <glm.hpp>
#include <vector>

#include "Core.h"

// useful functions
float RAYX_API hvlam(float x);

struct Rad;

// an angle in degrees.
struct Deg {
    Deg() = default;
    Deg(float d) : deg(d) {}

    Rad toRad() const;

    float deg;
};

// an angle in radians
struct Rad {
    Rad() = default;
    Rad(float r) : rad(r) {}

    Deg toDeg() const;
    float sin() const;
    float cos() const;
    float tan() const;

    float rad;
};

glm::mat4x4 getRotationMatrix(float dpsi, float dphi, float dchi);

void printDVec4(glm::vec4 vec);
void RAYX_API printDMat4(glm::mat4 matrix);
void printDMatrix(std::array<float, 4 * 4> matrix);
bool RAYX_API isIdentMatrix(glm::mat4x4 matrix);

std::array<float, 4 * 4> glmToArray16(glm::mat4x4 m);
glm::mat4x4 RAYX_API arrayToGlm16(std::array<float, 4 * 4> m);

std::array<float, 4> glmToArray4(glm::vec4 v);
glm::vec4 RAYX_API arrayToGlm4(std::array<float, 4> v);

std::vector<float>::iterator movingAppend(std::vector<float>&& srcVector, std::vector<float>& destVector);
