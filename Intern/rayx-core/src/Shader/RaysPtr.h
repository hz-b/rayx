#pragma once

#include "Rays.h"

namespace rayx {

// this struct is analog to struct Rays. It contains pointers to the attribute arrays
struct RaysPtr {
#define X(type, name, flag) type* __restrict name;

    RAYX_X_MACRO_RAY_ATTR
#undef X

    RAYX_FN_ACC glm::dvec3 position(const int i) const { return glm::dvec3(position_x[i], position_y[i], position_z[i]); }
    RAYX_FN_ACC void position(const int i, const glm::dvec3 position) {
        position_x[i] = position.x;
        position_y[i] = position.y;
        position_z[i] = position.z;
    }

    RAYX_FN_ACC glm::dvec3 direction(const int i) const { return glm::dvec3(direction_x[i], direction_y[i], direction_z[i]); }
    RAYX_FN_ACC void direction(const int i, const glm::dvec3 direction) {
        direction_x[i] = direction.x;
        direction_y[i] = direction.y;
        direction_z[i] = direction.z;
    }

    RAYX_FN_ACC ElectricField electric_field(const int i) const {
        return ElectricField(electric_field_x[i], electric_field_y[i], electric_field_z[i]);
    }
    RAYX_FN_ACC void electric_field(const int i, const ElectricField electric_field) {
        electric_field_x[i] = electric_field.x;
        electric_field_y[i] = electric_field.y;
        electric_field_z[i] = electric_field.z;
    }
};

}  // namespace rayx
