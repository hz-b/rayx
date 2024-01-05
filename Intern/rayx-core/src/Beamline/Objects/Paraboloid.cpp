#include "Paraboloid.h"

#include <ext/scalar_constants.hpp>

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"
#include "angle.h"

namespace RAYX {

Element makeParaboloid(const DesignObject& dobj) {
    auto ArmLength = dobj.parseArmLength();
    auto parameterP = dobj.parseParameterP();
    auto parameterPType = dobj.parseParameterPType();
    auto grazingIncAngle = dobj.parseGrazingIncAngle();
    auto a11 = dobj.parseParameterA11();

    double a24, a34, a44, y0, z0;
    //---------- Calculation will be outsourced ----------------
    int sign = parameterPType == 0 ? 1 : -1;  // 0:collimate, 1:focussing

    double sin1 = sin(2 * grazingIncAngle.rad);
    double cos1 = cos(2 * grazingIncAngle.rad);  // Schaefers RAY-Book may have a different calculation

    y0 = ArmLength * sin1;
    z0 = ArmLength * cos1 * sign;

    a24 = -y0;
    a34 = -parameterP;
    a44 = pow(y0, 2) - 2 * parameterP * z0 - pow(parameterP, 2);
    //-----------------------------------------------------------
    auto surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1.0,
        .m_a23 = 0,
        .m_a24 = a24,
        .m_a33 = 0,
        .m_a34 = a34,
        .m_a44 = a44,
    });
    auto behaviour = serializeMirror();
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
