#include "Cone.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {
Cone::Cone(const DesignObject& dobj) : OpticalElement(dobj) {
    m_incidence = dobj.parseGrazingIncAngle();
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();

    double zl = dobj.parseTotalLength();

    calcConeParams(zl);

    m_cm = pow((m_upstreamRadius_R - m_downstreamRadius_rho) / zl, 2);

    int icurv = 0;
    m_a11 = 1 - m_cm;
    m_a22 = 1 - 2 * m_cm;
    m_a23 = sqrt(m_cm - m_cm * m_cm);
    if (m_a22 > 0) icurv = 1;
    if (m_a23 != 0) {
        m_a24 = -m_a23 * (m_upstreamRadius_R / sqrt(m_cm) - zl / 2);
    } else if (m_a23 == 0) {
        m_a24 = -m_upstreamRadius_R;
    }

    m_surface = serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = m_a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = m_a22,
        .m_a23 = m_a23,
        .m_a24 = m_a24,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Cone::~Cone() = default;

/**
 * @brief Calculate of R and RHO For Cone from given Theta Entrance- and exit
 * arm lengths and length of mirror
 *
 * @param zl Total length
 * @remark Taken from RAY.FOR
 */
void Cone::calcConeParams(double zl) {
    double ra = m_entranceArmLength;
    double rb = m_exitArmLength;
    Rad th = m_incidence;

    double zl2 = pow(zl / 2, 2);
    double sth = th.sin();
    double cth = th.cos();
    double rmax1 = sqrt(zl2 + pow(ra, 2) - zl * ra * cth);
    double rmax2 = sqrt(zl2 + pow(rb, 2) + zl * rb * cth);
    double rmin1 = sqrt(zl2 + pow(ra, 2) + zl * ra * cth);
    double rmin2 = sqrt(zl2 + pow(rb, 2) - zl * rb * cth);
    double thmax = asin(ra * sth / rmax1);
    double thmin = asin(ra * sth / rmin1);
    double sthmax = sin(thmax);
    double sthmin = sin(thmin);

    m_upstreamRadius_R = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    m_downstreamRadius_rho = 2 * sthmin / (1 / rmin1 + 1 / rmin2);
}

Rad Cone::getIncidenceAngle() const { return m_incidence; }
double Cone::getEntranceArmLength() const { return m_entranceArmLength; }
double Cone::getExitArmLength() const { return m_exitArmLength; }
double Cone::getR() const { return m_upstreamRadius_R; }
double Cone::getRHO() const { return m_downstreamRadius_rho; }
}  // namespace RAYX
