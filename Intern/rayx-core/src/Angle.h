#pragma once

#include <glm.hpp>

#include <array>
#include <vector>

#include "Core.h"

namespace RAYX {

struct Rad;

// an angle in degrees.
struct RAYX_API Deg {
    Deg() = default;
    Deg(double d) : deg(d) {}

    Rad toRad() const;

    double deg;
};

// an angle in radians
struct RAYX_API Rad {
    Rad() = default;
    Rad(double r) : rad(r) {}

    Deg toDeg() const;
    double sin() const;
    double cos() const;
    double tan() const;

    double rad;
};

}  // namespace RAYX
