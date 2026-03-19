#include "ToModel.h"

#include "Beamline.h"

namespace rayx::detail {

model::Beamline toModel(const Beamline& beamline) {
    // explicitly call the toModel template function to avoid calling ourself
    return toModel<Beamline>(beamline);
}

}  // namespace rayx::detail
