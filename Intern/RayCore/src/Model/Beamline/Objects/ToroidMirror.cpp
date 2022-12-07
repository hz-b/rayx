#include "ToroidMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Model/Surface/Toroid.h"

namespace RAYX {
ToroidMirror::ToroidMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_longRadius = dobj.parseLongRadius();
    m_shortRadius = dobj.parseShortRadius();

    Material mat = dobj.parseMaterial();
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Toroid>(glm::dmat4x4{m_longRadius, m_shortRadius, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, TY_TOROID_MIRROR, 0, matd, 0}));
}

}  // namespace RAYX
