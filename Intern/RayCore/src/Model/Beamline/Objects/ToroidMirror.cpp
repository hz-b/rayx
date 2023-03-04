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

    setSurface(std::make_unique<Toroid>(m_longRadius, m_shortRadius));
}

}  // namespace RAYX
