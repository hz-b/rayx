#include "Cone.h"

#include "Debug.h"

namespace RAYX {
/**
 * @brief Construct a new Cone:: Cone object (R and RHO are automatically
 * calculated)
 *
 * @param name
 * @param geometricalShape
 * @param width         width of the mirror (x-dimension)
 * @param height        height of the mirror (z-dimension)
 * @param azimuthalAngle
 * @param position      position of the element in world coordinates
 * @param orientation   orientation of the element in world coordinates
 * @param grazingIncidence desired incidence angle of the main ray
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength length of exit arm
 * @param slopeError    7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 */
Cone::Cone(const char* name, Geometry::GeometricalShape geometricalShape,
           const double width, const double height, const double azimuthalAngle,
           glm::dvec4 position, glm::dmat4x4 orientation,
           const double grazingIncidence, const double entranceArmLength,
           const double exitArmLength, const std::vector<double> slopeError)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength) {
    RAYX_LOG << name << " :Auto";
    calcConePar(width, entranceArmLength, exitArmLength, grazingIncidence,
                &m_upstreamRadius_R, &m_downstreamRadius_rho);

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
    setSurface(std::make_unique<Quadric>(std::vector<double>{
        m_a11, 0, 0, 0, icurv, m_a22, m_a23, m_a24, 0, 0, 0, 0, 0, 0, 0, 0}));
    setElementParameters({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}
/**
 * @brief Construct a new Cone:: Cone object (R and RHO are also given)
 *
 * @param name
 * @param geometricalShape
 * @param upstream_radius_r R upstream Radius
 * @param downstream_radius_rho RHO downstream Radius
 * @param width             width of the mirror (x-dimension)
 * @param height            height of the mirror (z-dimension)
 * @param azimuthalAngle
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 */
Cone::Cone(const char* name, Geometry::GeometricalShape geometricalShape,
           const double upstream_radius_r, const double downstream_radius_rho,
           const double width, const double height, const double azimuthalAngle,
           glm::dvec4 position, glm::dmat4x4 orientation,
           const double grazingIncidence, const double entranceArmLength,
           const double exitArmLength, const std::vector<double> slopeError)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength),
      m_downstreamRadius_rho(downstream_radius_rho),
      m_upstreamRadius_R(upstream_radius_r) {
    RAYX_LOG << name << " :Manual";
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
    setSurface(std::make_unique<Quadric>(std::vector<double>{
        m_a11, 0, 0, 0, icurv, m_a22, m_a23, m_a24, 0, 0, 0, 0, 0, 0, 0, 0}));
    setElementParameters({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}

Cone::~Cone() {}

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
void Cone::calcConePar(const double zl, const double ra, const double rb,
                       const double th, double* R, double* RHO) {
    double zl2 = pow(zl / 2, 2);
    double sth = sin(th);
    double cth = sin(th);
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
double Cone::getIncidenceAngle() const { return m_incidence; }
double Cone::getEntranceArmLength() const { return m_entranceArmLength; }
double Cone::getExitArmLength() const { return m_exitArmLength; }
double Cone::getR() const { return m_upstreamRadius_R; }
double Cone::getRHO() const { return m_downstreamRadius_rho; }
}  // namespace RAYX