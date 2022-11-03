#include "SphereMirror.h"

#include "Debug.h"

namespace RAYX {

SphereMirror::SphereMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();
    m_grazingIncidenceAngle = dobj.parseGrazingIncAngle();

    calcRadius();  // calculate the radius
    Material mat = dobj.parseMaterial();
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{1, 0, 0, 0, 1, 1, 0, -m_radius, 0, 0, 1, 0, 0, 0, matd, 0}));
}

// TODO(Theresa): move this to user params and just give the radius as a
// parameter to the sphere class?
void SphereMirror::calcRadius() { m_radius = 2.0 / m_grazingIncidenceAngle.sin() / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength); }

double SphereMirror::getRadius() const { return m_radius; }

double SphereMirror::getExitArmLength() const { return m_exitArmLength; }

double SphereMirror::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
