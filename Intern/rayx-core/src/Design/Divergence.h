#pragma once

#include <variant>

#include "Angle.h"
#include "Distributions.h"

namespace rayx {

// ray direction is proportional to ray origin
struct CollimatedAngularDivergence {
    Angle horizontalDivergence;
    Angle verticalDivergence;
};

struct IndividualAngularDivergence {
    AngularAreaDistribution distribution;
};

using AngularDivergence = std::variant<CollimatedAngularDivergence, IndividualAngularDivergence>;

}  // namespace rayx
