#include "Cone.h"

#include "Debug.h"

namespace RAYX {
Cone::Cone(const DesignObject& dobj) : OpticalElement(dobj) {
    m_incidence = dobj.parseGrazingIncAngle();
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();

    double width = dobj.parseTotalWidth();

    calcConePar(width, m_entranceArmLength, m_exitArmLength, m_incidence, &m_upstreamRadius_R, &m_downstreamRadius_rho);

    m_cm = pow((m_upstreamRadius_R - m_downstreamRadius_rho) / width, 2);

    double icurv = 0;
    m_a11 = 1 - m_cm;
    m_a22 = 1 - 2 * m_cm;
    m_a23 = sqrt(m_cm - m_cm * m_cm);
    if (m_a22 > 0) icurv = 1;
    if (m_a23 != 0) {
        m_a24 = -m_a23 * (m_upstreamRadius_R / sqrt(m_cm) - width / 2);
    } else if (m_a23 == 0) {
        m_a24 = -m_upstreamRadius_R;
    }
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{m_a11, 0, 0, 0, icurv, m_a22, m_a23, m_a24, 0, 0, 0, 0, 0, 0, 0, 0}));
}

Cone::~Cone() = default;

/**
 * @brief Calculate of R and RHO For Cone from given Theta Entrance- and exit
 * arm lengths and length of mirror
 *
 * @param zl Total length
 * @param ra EntranceArmLength
 * @param rb ExitArmLength
 * @param th GrazingIncidenceAngle
 * @param R UpstreamRadius
 * @param RHO DownstreamRadius
 * @remark Taken from RAY.FOR
 */
void Cone::calcConePar(double zl, double ra, double rb, Rad th, double* R, double* RHO) {
    double zl2 = pow(zl / 2, 2);
    double sth = th.sin();
    double cth = th.cos();  // TODO this was originally th.sin() aswell.
    double rmax1 = sqrt(zl2 + pow(ra, 2) - zl * ra * cth);
    double rmax2 = sqrt(zl2 + pow(rb, 2) + zl * rb * cth);
    double rmin1 = sqrt(zl2 + pow(ra, 2) + zl * ra * cth);
    double rmin2 = sqrt(zl2 + pow(rb, 2) - zl * rb * cth);
    double thmax = asin(ra * sth / rmax1);
    double thmin = asin(ra * sth / rmin1);
    double sthmax = sin(thmax);
    double sthmin = sin(thmin);
    *R = 2 * sthmax / (1 / rmax1 + 1 / rmax2);
    *RHO = 2 * sthmin / (1 / rmin1 + 1 / rmin2);
}

Rad Cone::getIncidenceAngle() const { return m_incidence; }
double Cone::getEntranceArmLength() const { return m_entranceArmLength; }
double Cone::getExitArmLength() const { return m_exitArmLength; }
double Cone::getR() const { return m_upstreamRadius_R; }
double Cone::getRHO() const { return m_downstreamRadius_rho; }
}  // namespace RAYX
