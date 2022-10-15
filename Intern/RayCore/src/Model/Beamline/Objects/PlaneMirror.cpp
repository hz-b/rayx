#include "PlaneMirror.h"

#include <Data/xml.h>

#include "Debug.h"

namespace RAYX {

PlaneMirror::PlaneMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    double widthB = 0.0;
    xml::paramDouble(dobj.node, "totalWidthB", &widthB);

    m_Geometry->m_geometricalShape = dobj.parseGeometricalShape();
    m_Geometry->setHeightWidth(dobj.parseTotalLength(), dobj.parseTotalWidth(), widthB);
    m_Geometry->m_azimuthalAngle = dobj.parseAzimuthalAngle();
    m_Geometry->m_position = dobj.parsePosition();
    m_Geometry->m_orientation = dobj.parseOrientation();

    auto matd = (double)static_cast<int>(dobj.parseMaterial());
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, matd, 0}));
}

}  // namespace RAYX
