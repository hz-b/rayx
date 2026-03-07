#pragma once

#include "Model/Beamline.h"

namespace rayx {

struct Beamline;

}

namespace rayx::detail {

model::Beamline toModel(const Beamline& beamline);

}
