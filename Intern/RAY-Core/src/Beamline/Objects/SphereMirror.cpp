#include "SphereMirror.h"

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
    double azim;
    try {
        azim = dobj.parseAzimuthalAngle().rad;
    } catch (std::runtime_error& err) {
        azim = 0;
    }

    return Element{
        .m_inTrans = defaultInMatrix(dobj),
        .m_outTrans = defaultOutMatrix(dobj),
        .m_behaviour = serializeMirror(),
        .m_surface = surface,
        .m_cutout = dobj.parseCutout(),
        .m_slopeError = {m_slopeError[0], m_slopeError[1], m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5], m_slopeError[6]},
        .m_azimuthalAngle = azim,
        .m_material = (double)static_cast<int>(dobj.parseMaterial()),
        .m_padding = {0.0},
    };
}

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
    m_behaviour = serializeMirror();
}

// TODO(Theresa): move this to user params and just give the radius as a
// parameter to the sphere class?
void SphereMirror::calcRadius() { m_radius = 2.0 / m_grazingIncidenceAngle.sin() / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength); }

double SphereMirror::getRadius() const { return m_radius; }

double SphereMirror::getExitArmLength() const { return m_exitArmLength; }

double SphereMirror::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
