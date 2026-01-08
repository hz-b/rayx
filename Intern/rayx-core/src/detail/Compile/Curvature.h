#pragma once

#include "Design/Curvature.h"

namespace rayx::detail::designToHost {

using CompiledCurvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature>;

CompiledCurvature compileCurvature(const Curvature& curvature);

}  // namespace rayx::detail
