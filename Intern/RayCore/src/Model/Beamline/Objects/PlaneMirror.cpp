#include "PlaneMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0}));
}

}  // namespace RAYX
