#pragma once

#include "Design/Curvature.h"

namespace rayx::detail::host {

using QuadricCurvature   = design::QuadricCurvature;
using ToroidialCurvature = design::ToroidialCurvature;
using CubicCurvature     = design::CubicCurvature;
using Curvature = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature>;

Curvature toHost(const design::Curvature& curvature);

}  // namespace rayx::detail::host
