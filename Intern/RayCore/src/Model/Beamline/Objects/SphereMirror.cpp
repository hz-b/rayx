#include "SphereMirror.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

SphereMirror::SphereMirror(const DesignObject& dobj) : OpticalElement(dobj) {
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();
    m_grazingIncidenceAngle = dobj.parseGrazingIncAngle();

    calcRadius();  // calculate the radius

    m_surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 1,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = -m_radius,
        .m_a33 = 1,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

// TODO(Theresa): move this to user params and just give the radius as a
// parameter to the sphere class?
void SphereMirror::calcRadius() { m_radius = 2.0 / m_grazingIncidenceAngle.sin() / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength); }

double SphereMirror::getRadius() const { return m_radius; }

double SphereMirror::getExitArmLength() const { return m_exitArmLength; }

double SphereMirror::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
