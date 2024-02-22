#include "ToroidMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeToroidMirror(const DesignElement& dele) {
    auto surface = makeToroidEle(dele);
    auto behaviour = serializeMirror();
    return makeDesElement(dele, behaviour, surface);
}

}  // namespace RAYX
