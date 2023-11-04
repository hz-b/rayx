#include "ToroidMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeToroidMirror(const DesignObject& dobj) {
    auto surface = makeToroid(dobj);
    auto behaviour = serializeMirror();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
