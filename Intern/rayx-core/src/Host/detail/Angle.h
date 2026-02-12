#pragma once

#include "Design/Angle.h"

namespace rayx::host::detail {

double toHost(const design::Deg degrees) { return design::toRad(degrees).value; }

double toHost(const design::Rad radians) { return design::toRad(radians).value; }

double toHost(const design::Angle angle) { return design::toRad(angle).value; }

}  // namespace rayx::host::detail
