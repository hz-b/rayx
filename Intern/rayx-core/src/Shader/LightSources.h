#pragma once

#include <variant>

#include "LightSources/CircleSource.h"
#include "LightSources/DipoleSource.h"
#include "LightSources/MatrixSource.h"
#include "LightSources/PixelSource.h"
#include "LightSources/PointSource.h"
#include "LightSources/SimpleUndulatorSource.h"

namespace RAYX {

struct InvState;

struct BatchInputRays {};

using LightSourceVariant =
    std::variant<std::monostate, MatrixSource, CircleSource, DipoleSource, PixelSource, PointSource, SimpleUndulatorSource, BatchInputRays>;

RAYX_FN_ACC Ray lightSourceGetRay(const InvState& inv, Rand& rand);

}  // namespace RAYX
