#include "ToroidMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"

namespace RAYX {
ToroidMirror::ToroidMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_longRadius = dobj.parseLongRadius();
    m_shortRadius = dobj.parseShortRadius();

    m_surfaceType = STYPE_TOROID;
    m_surfaceParams = {m_longRadius, m_shortRadius};
}

}  // namespace RAYX
