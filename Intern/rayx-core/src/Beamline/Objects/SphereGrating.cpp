#include "SphereGrating.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"

namespace RAYX {

Element makeSphereGrating(const DesignElement& dele) {
    auto surface = makeSphere(dele.getRadius());
    auto behaviour = makeGratingEle(dele);
    return makeDesElement(dele, behaviour, surface);
}

}  // namespace RAYX
