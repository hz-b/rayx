#include "PlaneMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    auto matd = (double)static_cast<int>(dobj.parseMaterial());
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, TY_BASIC_MIRROR, 0, matd, 0}));
}

}  // namespace RAYX
