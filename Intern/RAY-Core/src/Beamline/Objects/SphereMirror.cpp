#include "SphereMirror.h"

#include "Beamline/OpticalElement.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeSphereMirror(const DesignObject& dobj) {
    auto entranceArmLength = dobj.parseEntranceArmLength();
    auto exitArmLength = dobj.parseExitArmLength();
    auto grazingIncidenceAngle = dobj.parseGrazingIncAngle();
    auto radius = 2.0 / grazingIncidenceAngle.sin() / (1.0 / entranceArmLength + 1.0 / exitArmLength);
    auto surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 1,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = -radius,
        .m_a33 = 1,
        .m_a34 = 0,
        .m_a44 = 0,
    });
    auto m_slopeError = dobj.parseSlopeError();

    return Element{
        .m_inTrans = defaultInMatrix(dobj),
        .m_outTrans = defaultOutMatrix(dobj),
        .m_behaviour = serializeMirror(),
        .m_surface = surface,
        .m_cutout = dobj.parseCutout(),
        .m_slopeError = {m_slopeError[0], m_slopeError[1], m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5], m_slopeError[6]},
        .m_azimuthalAngle = defaultAzimuthalAngle(dobj).rad,
        .m_material = defaultMaterial(dobj),
        .m_padding = {0.0},
    };
}

}  // namespace RAYX
