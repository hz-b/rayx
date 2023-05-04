#include "Cone.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {
Element makeCone(const DesignObject& dobj) {
    auto element = defaultElement(dobj);

    auto incidence = dobj.parseGrazingIncAngle();
    double entranceArmLength = dobj.parseEntranceArmLength();
    double exitArmLength = dobj.parseExitArmLength();

    double zl = dobj.parseTotalLength();

    double ra = entranceArmLength;
    double rb = exitArmLength;

    double zl2 = pow(zl / 2, 2);
    double sth = incidence.sin();
    double cth = incidence.cos();
    double rmax1 = sqrt(zl2 + pow(ra, 2) - zl * ra * cth);
    double rmax2 = sqrt(zl2 + pow(rb, 2) + zl * rb * cth);
    double rmin1 = sqrt(zl2 + pow(ra, 2) + zl * ra * cth);
    double rmin2 = sqrt(zl2 + pow(rb, 2) - zl * rb * cth);
    double thmax = asin(ra * sth / rmax1);
    double thmin = asin(ra * sth / rmin1);
    double sthmax = sin(thmax);
    double sthmin = sin(thmin);

    double upstreamRadius_R = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    double downstreamRadius_rho = 2 * sthmin / (1 / rmin1 + 1 / rmin2);

    auto cm = pow((upstreamRadius_R - downstreamRadius_rho) / zl, 2);

    int icurv = 0;
    double a11 = 1 - cm;
    double a22 = 1 - 2 * cm;
    double a23 = sqrt(cm - cm * cm);
    double a24 = 0;  //  TODO correct default?

    if (a22 > 0) icurv = 1;
    if (a23 != 0) {
        a24 = -a23 * (upstreamRadius_R / sqrt(cm) - zl / 2);
    } else if (a23 == 0) {
        a24 = -upstreamRadius_R;
    }

    element.m_surface = serializeQuadric({
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
    element.m_behaviour = serializeMirror();
    return element;
}
}  // namespace RAYX
