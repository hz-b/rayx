#include "Cylinder.h"

#include <ext/scalar_constants.hpp>

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"
#include "angle.h"

namespace RAYX {

Element makeCylinder(const DesignObject& dobj) {
    auto cyl_direction = dobj.parseBendingRadius();
    auto radius = dobj.parseRadius();
    auto incidence = dobj.parseGrazingIncAngle();
    auto entranceArmLength = dobj.parseEntranceArmLength();
    auto exitArmLength = dobj.parseExitArmLength();

    double a11 = 0, a33 = 0, a24 = 0;
    if (cyl_direction == CylinderDirection::LongRadiusR) {  // X-DIR
        a11 = 0;
        a33 = 1;
        a24 = -radius;
    } else {  // Z_DIR/ SHORT_RADIUS_RHO
        a11 = 1;
        a33 = 0;
        a24 = -radius;
    }
    int icurv = 1;
    if (a24 > 0) icurv = -1;  // Translated from RAY.FOR
    if (radius == 0) {
        if (cyl_direction == CylinderDirection::LongRadiusR) {
            radius = 2.0 / incidence.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);
        } else {
            if (entranceArmLength == 0.0 || exitArmLength == 0.0 || incidence.rad == 0.0) {
                radius = 0.0;
            } else {
                radius = 2.0 * incidence.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);
            }
        }
    }

    auto surface = serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = a24,
        .m_a33 = a33,
        .m_a34 = 0,
        .m_a44 = 0,
    });
    auto behaviour = serializeMirror();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
