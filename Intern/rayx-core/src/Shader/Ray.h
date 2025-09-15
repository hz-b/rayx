#pragma once

#include <glm.hpp>

#include "Core.h"
#include "ElectricField.h"
#include "EventType.h"
#include "Rand.h"

namespace RAYX {

// this struct is for the usage inside a kernel
// it is padded for optimal memory access on the CPU
// the GPU does not care about padding as long as Ray stays in registers
struct Ray {
    // keep position and direction together for better cache performance, since they are likely to be used together
    glm::dvec3 position;
    glm::dvec3 direction;

    double energy;
    double optical_path_length;

    ElectricField electric_field;

    Rand rand;  // deletes copy constructor/assignment

    int path_id;
    int path_event_id;
    int order;
    int object_id;
    int source_id;
    EventType event_type;
};

// make sure Ray is not copy constructable/assignable to protect it from costly copies
static_assert(!std::is_copy_constructible_v<Ray> && !std::is_copy_assignable_v<Ray>);

}  // namespace RAYX
