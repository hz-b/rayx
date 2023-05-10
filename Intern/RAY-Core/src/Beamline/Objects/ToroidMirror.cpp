#include "ToroidMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeToroidMirror(const DesignObject& dobj) {
    auto surface = serializeToroid({
        .m_longRadius = dobj.parseLongRadius(),
        .m_shortRadius = dobj.parseShortRadius(),
    });
    auto behaviour = serializeMirror();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
