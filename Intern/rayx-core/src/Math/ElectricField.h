#pragma once

#include <glm/glm.hpp>
#include "Complex.h"

namespace rayx::math {

using Stokes = glm::dvec4;

using LocalElectricField = glm::tvec2<Complex>;
using ElectricField = glm::tvec3<Complex>;

}  // namespace rayx::math
