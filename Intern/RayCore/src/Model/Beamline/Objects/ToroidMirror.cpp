#include "ToroidMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {
ToroidMirror::ToroidMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_surface = serializeToroid({
        .m_longRadius = dobj.parseLongRadius(),
        .m_shortRadius = dobj.parseShortRadius(),
    });
}

}  // namespace RAYX
