#include "PlaneMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    setSurface(std::make_unique<Quadric>(std::array<double, 16>{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0}));
}

}  // namespace RAYX
