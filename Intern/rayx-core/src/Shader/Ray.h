#pragma once

#include <glm.hpp>

#include "Core.h"
#include "ElectricField.h"
#include "EventType.h"

namespace RAYX {

// this struct is menat, only for the usage inside a kernel
struct Ray {
    int path_id;
    int event_id;
    glm::dvec3 position;
    EventType event_type;
    glm::dvec3 direction;
    double energy;
    ElectricField electric_field;
    double optical_path_length;
    int order;
    int object_id;
    int source_id;
    Rand rand;  // inherently deletes copy ctor of Ray
};

// make sure Ray is not copy constructable/assignable to protect us from doing costly copies
static_assert(!std::is_copy_constructible_v<Ray> && !std::is_copy_assignable_v<Ray>);

}  // namespace RAYX
