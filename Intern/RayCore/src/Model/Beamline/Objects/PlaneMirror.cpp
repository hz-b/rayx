#include "PlaneMirror.h"

#include "Data/xml.h"
#include "Shared/Constants.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surfaceType = STYPE_QUADRIC;
    m_surfaceParams = {0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0};
}

}  // namespace RAYX
