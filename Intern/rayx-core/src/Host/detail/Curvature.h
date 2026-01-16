#pragma once

#include "Design/Curvature.h"

namespace rayx::host::detail {

using QuadricCurvature   = design::QuadricCurvature;
using ToroidialCurvature = design::ToroidialCurvature;
using ToroidType         = design::ToroidType;
using CubicCurvature     = design::CubicCurvature;
using Curvature          = std::variant<QuadricCurvature, ToroidialCurvature, CubicCurvature>;

Curvature toHost(const design::Curvature& curvature);

}
