#pragma once

#include "Design/Angle.h"

namespace rayx::host::detail {

double toHost(const design::Degrees degrees) {
    return design::toRadians(degrees).value;
}

double toHost(const design::Radians radians) {
    return design::toRadians(radians).value;
}

double toHost(const design::Angle angle) {
    return design::toRadians(angle).value;
}

}
