#include "ToroidMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeToroidMirror(const DesignElement& dele) {
    auto surface = makeToroid(dele);
    auto behaviour = serializeMirror();
    return makeElement(dele, behaviour, surface);
}

}  // namespace RAYX
