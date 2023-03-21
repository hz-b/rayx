#include "SphereMirror.h"

#include "Constants.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"

namespace RAYX {

SphereMirror::SphereMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();
    m_grazingIncidenceAngle = dobj.parseGrazingIncAngle();

    calcRadius();  // calculate the radius
    m_surfaceType = STYPE_QUADRIC;
    m_surfaceParams = {1, 0, 0, 0, 1, 1, 0, -m_radius, 0, 0, 1, 0, 0, 0, 0, 0};
}

// TODO(Theresa): move this to user params and just give the radius as a
// parameter to the sphere class?
void SphereMirror::calcRadius() { m_radius = 2.0 / m_grazingIncidenceAngle.sin() / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength); }

double SphereMirror::getRadius() const { return m_radius; }

double SphereMirror::getExitArmLength() const { return m_exitArmLength; }

double SphereMirror::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
