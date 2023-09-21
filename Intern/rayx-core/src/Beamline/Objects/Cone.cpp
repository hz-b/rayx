#include "Cone.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {
Element makeCone(const DesignObject& dobj) {
    auto incidence = dobj.parseGrazingIncAngle();
    float entranceArmLength = dobj.parseEntranceArmLength();
    float exitArmLength = dobj.parseExitArmLength();

    float zl = dobj.parseTotalLength();

    float ra = entranceArmLength;
    float rb = exitArmLength;

    float zl2 = pow(zl / 2, 2);
    float sth = incidence.sin();
    float cth = incidence.cos();
    float rmax1 = sqrt(zl2 + pow(ra, 2) - zl * ra * cth);
    float rmax2 = sqrt(zl2 + pow(rb, 2) + zl * rb * cth);
    float rmin1 = sqrt(zl2 + pow(ra, 2) + zl * ra * cth);
    float rmin2 = sqrt(zl2 + pow(rb, 2) - zl * rb * cth);
    float thmax = asin(ra * sth / rmax1);
    float thmin = asin(ra * sth / rmin1);
    float sthmax = sin(thmax);
    float sthmin = sin(thmin);

    float upstreamRadius_R = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    float downstreamRadius_rho = 2 * sthmin / (1 / rmin1 + 1 / rmin2);

    auto cm = pow((upstreamRadius_R - downstreamRadius_rho) / zl, 2);

    int icurv = 0;
    float a11 = 1 - cm;
    float a22 = 1 - 2 * cm;
    float a23 = sqrt(cm - cm * cm);
    float a24 = 0;  //  TODO correct default?

    if (a22 > 0) icurv = 1;
    if (a23 != 0) {
        a24 = -a23 * (upstreamRadius_R / sqrt(cm) - zl / 2);
    } else if (a23 == 0) {
        a24 = -upstreamRadius_R;
    }

    auto behaviour = serializeMirror();
    auto surface = serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = a22,
        .m_a23 = a23,
        .m_a24 = a24,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
    return makeElement(dobj, behaviour, surface);
}
}  // namespace RAYX
