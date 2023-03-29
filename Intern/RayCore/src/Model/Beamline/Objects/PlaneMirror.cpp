#include "PlaneMirror.h"

#include "Data/xml.h"
#include "Shared/Constants.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 0,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 0,
        .m_a23 = 0,
        .m_a24 = -1,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
    m_behaviour = serializeMirror();
}

}  // namespace RAYX
