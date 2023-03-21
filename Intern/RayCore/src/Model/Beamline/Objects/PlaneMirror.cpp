#include "PlaneMirror.h"

#include "Constants.h"
#include "Data/xml.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surfaceType = STYPE_QUADRIC;
    m_surfaceParams = {0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0};
}

}  // namespace RAYX
