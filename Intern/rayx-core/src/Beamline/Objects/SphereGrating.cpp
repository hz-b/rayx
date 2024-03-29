#include "SphereGrating.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeSphereGrating(const DesignObject& dobj) {
    auto surface = makeSphere(dobj.parseRadius());
    auto behaviour = makeGrating(dobj);
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
