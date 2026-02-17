#pragma once

#include "Design/Beamline.h"
#include "ToModel.h"

namespace rayx::detail::model {

struct Beamline {

};

}

namespace rayx::detail {

inline model::Beamline toModel(const Beamline& beamline) {
    return model::Beamline();
}

}
