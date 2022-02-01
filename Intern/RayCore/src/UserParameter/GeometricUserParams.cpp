#include "GeometricUserParams.h"

#include "Debug.h"

namespace RAYX {
/**
 * for Plane and Sphere Mirrors
 * @param incidenceAngle        incidence Angle in degree
 */
GeometricUserParams::GeometricUserParams(double incidenceAngle)
    : m_alpha(degToRad(incidenceAngle)),
      m_beta(degToRad(incidenceAngle)),
      m_radius(0),
      m_shortRadius(0) {}

/**
 * Plane + Sphere Grating
 * angles in degree
 * @param mount                 0 = constant deviation, 1 = constant incidence
 * @param deviation             deviation angle (= (alpha + abs(beta)) / 2)
 * @param normalIncidence       normal incidence angle
 * @param lineDensity           line density of grating in lines/mm
 * @param designEnergy          energy for which the grating is designed,
 * usually source energy
 * @param additionalOrder       0/1 whether or not to trace the zero order
 * @param orderOfDiffraction    order of diffraction that should be traced
 */
GeometricUserParams::GeometricUserParams(GratingMount mount, double deviation,
                                         double normalIncidence,
                                         double lineDensity,
                                         double designEnergy,
                                         int orderOfDiffraction)
    : m_radius(0), m_shortRadius(0) {
    double angle = 0;  // TODO what should angle be if mount is neither 0 or 1?
    if (mount == GratingMount::Deviation) {
        angle = deviation;
    } else if (mount == GratingMount::Incidence) {
        angle = -normalIncidence;
    }
    focus(angle, designEnergy, lineDensity, orderOfDiffraction);
}

// RZP
GeometricUserParams::GeometricUserParams(
    GratingMount mount, ImageType imageType, double deviationAngle,
    double grazingIncidence, double grazingExitAngle, double sourceEnergy,
    double designEnergy, double orderOfDiffraction,
    double designOrderOfDiffraction, double designAlphaAngle,
    double designBetaAngle, double mEntrance, double mExit, double sEntrance,
    double sExit)
    : m_radius(0), m_shortRadius(0) {
    double designWavelength = hvlam(designEnergy);
    double sourceWavelength = hvlam(sourceEnergy);
    double dz =
        calcDz00(imageType, designWavelength, designAlphaAngle, designBetaAngle,
                 designOrderOfDiffraction, sEntrance, sExit, mEntrance, mExit);

    GratingMount gratingMount = mount;
    // calculate alpha depending on either incidence or deviation angle IF
    // incidence not given directly
    if (grazingIncidence == 0) {
        if (gratingMount == GratingMount::Incidence) {
            RAYX_LOG << "use design angle";
            m_alpha = degToRad(designAlphaAngle);
        } else if (gratingMount == GratingMount::Deviation) {
            RAYX_LOG << "use deviation angle";
            focus(designEnergy, deviationAngle, dz, orderOfDiffraction);
        }
    } else {
        RAYX_LOG << "use incidence angle";
        m_alpha = degToRad(grazingIncidence);
    }

    if (grazingExitAngle == 0) {  // calculate from other parameters
        double DZ = (designOrderOfDiffraction == 0) ? 0 : dz;
        m_beta = acos(cos(m_alpha) -
                      orderOfDiffraction * sourceWavelength * 1e-6 * DZ);
    } else {  // auto == true
        m_beta = degToRad(grazingExitAngle);
    }
}

/**
 * for ellipsoid mirror
 * angles in degree
 *
 * @param incidence         grazing incidence angle
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 */
GeometricUserParams::GeometricUserParams(double incidence,
                                         double entranceArmLength,
                                         double exitArmLength) {
    double tangentAngle =
        calcTangentAngle(incidence, entranceArmLength, exitArmLength, 1);
    double incidenceAngle = degToRad(incidence);
    m_alpha = incidenceAngle - tangentAngle;
    m_beta = incidenceAngle + tangentAngle;
}

GeometricUserParams::GeometricUserParams() {}
GeometricUserParams::~GeometricUserParams() {}

void GeometricUserParams::focus(double angle, double designEnergy,
                                double lineDensity, double orderOfDiffraction) {
    // from routine "focus" in RAYX.FOR
    double theta = degToRad(abs(angle));
    double alph, bet;
    double a =
        abs(hvlam(designEnergy)) * abs(lineDensity) * orderOfDiffraction * 1e-6;
    RAYX_LOG << "deviation " << angle << "theta" << theta;
    if (angle <= 0) {  // constant alpha mounting
        double arg = a - sin(theta);
        if (abs(arg) >= 1) {  // cannot calculate alpha & beta
            alph = 0;
            bet = 0;
        } else {
            alph = theta;
            bet = asin(arg);
        }
    } else {  // constant alpha & beta mounting
        theta = theta / 2;
        double arg = a / 2 / cos(theta);
        if (abs(arg) >= 1) {
            alph = 0;
            bet = 0;
        } else {
            bet = asin(arg) - theta;
            alph = 2 * theta + bet;
        }
    }
    RAYX_LOG << alph << ", " << bet << " angles";
    m_alpha = (PI / 2 - alph);
    m_beta = (PI / 2 - abs(bet));
}

/**
 * needed for calculating exit angle beta for the RZP
 * @param imageType                 point to point vs astigmatic 2 astigmatic
 * @param designWavelength
 * @param designAlphaAngle          design incidence angle in degree
 * @param designBetaAngle           design exit angle in degree
 * @param designOrderOfDiffraction
 * @param sEntrance                 sagittal entrance arm length
 * @param sExit                     sagittal exit arm length
 * @param mEntrance
 * @param mExit
 * @return Dz00 = line density at 0,0??
 */
double GeometricUserParams::calcDz00(
    ImageType imageType, double designWavelength, double designAlphaAngle,
    double designBetaAngle, double designOrderOfDiffraction, double sEntrance,
    double sExit, double mEntrance, double mExit) {
    // double fresnelOffset = calcFresnelZOffset(designBetaAngle,
    // designAlphaAngle, sEntrance, sExit); // overwrite given Fresneloffset
    // RAYX-UI calls rzpLineDensity function in fortran
    double DZ = rzpLineDensityDZ(
        imageType, glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), designWavelength,
        degToRad(designAlphaAngle), degToRad(designBetaAngle),
        designOrderOfDiffraction, sEntrance, sExit, mEntrance, mExit);
    return DZ;
}

/**
 * Calculate fresnel z offset if DesignType == BETA from design angles.
 * @param designBetaAngle
 * @param designAlphaAngle
 * @param sEntrance             sagittal entrance arm length
 * @param sExit                 sagittal exit arm length
 * @return fresnelZOffset
 */
double GeometricUserParams::calcFresnelZOffset(double designAlphaAngle,
                                               double designBetaAngle,
                                               double sEntrance, double sExit) {
    double RIcosa = sEntrance * cos(designAlphaAngle);
    double ROcosb = sExit * cos(designBetaAngle);
    double RIsina = sEntrance * sin(designAlphaAngle);
    double ROsinb = sExit * sin(designBetaAngle);
    double tanTheta = (RIsina + ROsinb) / (RIcosa + ROcosb);
    return (RIsina / tanTheta) - RIcosa;
}

/**
 * needed for calculating exit angle of RZP
 * calculates the line density at one specific point (x,y,z) for a given normal
 * at this point and the design wavelength
 * @param imageType
 * @param intersection          coordinates of intersection point
 * @param normal                normal at intersection point
 * @param designWavelength      design wavelength of rzp
 * @param designAlphaAngle
 * @param designBetaAngle
 * @param designOrderOfDiffraction
 * @param sEntrance
 * @param sExit
 * @param mEntrance
 * @param mExit
 * @return line density on RZP in Z direction for given conditions
 */
double GeometricUserParams::rzpLineDensityDZ(
    ImageType imageType, glm::dvec3 intersection, glm::dvec3 normal,
    double designWavelength, double designAlphaAngle, double designBetaAngle,
    double designOrderOfDiffraction, double sEntrance, double sExit,
    double mEntrance, double mExit) {
    double s_beta = sin(designAlphaAngle);
    double c_beta = cos(designBetaAngle);
    double s_alpha = sin(designAlphaAngle);
    double c_alpha = cos(designBetaAngle);

    double risag = sEntrance;
    double rosag = sExit;
    double rimer = mEntrance;
    double romer = mExit;

    double DZ;  //, DX;

    // all of the upcoming variables will be defined in each of the following
    // if-else blocks!
    double xi = 0;
    double yi = 0;
    double zi = 0;
    double xm = 0;
    double ym = 0;
    double zm = 0;

    if (imageType == ImageType::Point2Point) {  // point to point (standard)
        if (normal.x == 0 && normal.z == 0) {   // plane
            zi = -(risag * c_alpha + intersection.z);
            xi = intersection.x;
            yi = risag * s_alpha;
            zm = rosag * c_beta - intersection.z;
            xm = intersection.x;
            ym = rosag * s_beta;
        } else {  // more general case, can be reduced to the plane with normal
                  // = (0,1,0) and y = 0
            zi = normal.x * normal.z * intersection.x -
                 (normal.x * normal.x + normal.y * normal.y) *
                     (intersection.z + risag * c_alpha) +
                 normal.y * normal.z * (intersection.y - risag * s_alpha);
            xi = (normal.y * intersection.x - normal.x * intersection.y +
                  normal.x * risag * s_alpha);
            yi = -(normal.x * intersection.x) - normal.y * intersection.y -
                 normal.z * intersection.z - normal.z * risag * c_alpha +
                 normal.y * risag * s_alpha;
            zm = normal.x * normal.z * intersection.x +
                 (normal.x * normal.x + normal.y * normal.y) *
                     (-intersection.z + rosag * c_beta) +
                 normal.y * normal.z * (intersection.y - rosag * s_beta);
            xm = (normal.y * intersection.x - normal.x * intersection.y +
                  normal.x * rosag * s_beta);
            ym = -(normal.x * intersection.x) - normal.y * intersection.y -
                 normal.z * intersection.z + normal.z * rosag * c_beta +
                 normal.y * rosag * s_beta;
        }
    } else if (imageType ==
               ImageType::Astigmatic2Astigmatic) {  // astigmatic to astigmatix
        double s_rim = rimer < 0 ? -1 : 1;
        double s_rom = romer < 0 ? -1 : 1;
        double c_2alpha = cos(2 * designAlphaAngle);
        double c_2beta = cos(2 * designBetaAngle);
        if (normal.x == 0 && normal.z == 0) {  //   !plane

            zi = s_rim * (rimer * c_alpha + intersection.z);
            xi = (s_rim * intersection.x *
                  (c_alpha * intersection.z - 2 * s_alpha * s_alpha * rimer +
                   s_alpha * intersection.y + rimer)) /
                 (c_alpha * intersection.z - 2 * s_alpha * s_alpha * risag +
                  s_alpha * intersection.y + risag);
            yi = s_rim * (-rimer * s_alpha + intersection.y);
            zm = s_rom * (romer * c_beta - intersection.z);
            xm = (s_rom * intersection.x *
                  (-c_beta * intersection.z - 2 * s_beta * s_beta * romer +
                   s_beta * intersection.y + romer)) /
                 (c_beta * intersection.z + 2 * s_beta * s_beta * rosag -
                  s_beta * intersection.y - rosag);
            ym = s_rom * (romer * s_beta - intersection.y);
        } else {
            double denominator = intersection.z * c_alpha + risag * c_2alpha +
                                 intersection.y * s_alpha;
            double nominator =
                intersection.x * (intersection.z * c_alpha + rimer * c_2alpha +
                                  intersection.y * s_alpha);
            zi = s_rim *
                 ((normal.x * normal.x + normal.y * normal.y) *
                      (intersection.z + rimer * c_alpha) -
                  normal.y * normal.z * (intersection.y - rimer * s_alpha) -
                  (normal.x * normal.z * nominator) / denominator);
            xi = s_rim *
                 (-(normal.x * intersection.y) + normal.x * rimer * s_alpha +
                  (normal.y * nominator) / denominator);
            yi = s_rim * (normal.z * (intersection.z + rimer * c_alpha) +
                          normal.y * (intersection.y - rimer * s_alpha) +
                          (normal.x * nominator) / denominator);

            denominator = (-(intersection.z * c_beta) + rosag * c_2beta +
                           intersection.y * s_beta);
            nominator =
                intersection.x * (-(intersection.z * c_beta) + romer * c_2beta +
                                  intersection.y * s_beta);
            zm = s_rom *
                 ((normal.x * normal.x + normal.y * normal.y) *
                      (-intersection.z + romer * c_beta) +
                  normal.y * normal.z * (intersection.y - romer * s_beta) +
                  (normal.x * normal.z * nominator) / denominator);
            xm = s_rom * (normal.x * (intersection.y - romer * s_beta) -
                          (normal.y * nominator) / denominator);
            ym = s_rom * (normal.z * (-intersection.z + romer * c_beta) +
                          normal.y * (-intersection.y + romer * s_beta) -
                          (normal.x * nominator) / denominator);
        }
        double ris = sqrt(zi * zi + xi * xi + yi * yi);
        double rms = sqrt(zm * zm + xm * xm + ym * ym);

        double ai = zi / ris;
        double am = -zm / rms;

        DZ = (ai + am) / (designWavelength * designOrderOfDiffraction);

        return DZ;
    }

    double ris = sqrt(zi * zi + xi * xi + yi * yi);
    double rms = sqrt(zm * zm + xm * xm + ym * ym);

    // double ai = xi / ris;
    // double am = xm / rms;
    double bi = zi / ris;
    double bm = zm / rms;

    // DX = (ai + am) / (WL * m_designOrderOfDiffraction);
    DZ = (-bi - bm) / (designWavelength * designOrderOfDiffraction);

    return DZ;
}

// ellipsoid method
double GeometricUserParams::calcTangentAngle(double incidence,
                                             double entranceArmLength,
                                             double exitArmLength,
                                             int coordSys) {
    if (coordSys == 0)
        return 0;  // ellipsoid coord sys. else mirror coordinate system
    double theta =
        degToRad(incidence);  // designGrazingIncidenceAngle always equal to
                              // alpha (grazingIncidenceAngle)??
    if (theta > PI / 2) {
        theta = PI / 2;
    }
    double angle = atan(tan(theta) * (entranceArmLength - exitArmLength) /
                        (entranceArmLength + exitArmLength));
    return angle;
    // RAYX_LOG << "A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB <<
    // ", C= " << m_halfAxisC << ", angle = " << m_tangentAngle << ", Z0 = " <<
    // m_z0 << ", Y0= " << m_y0;
}

// calculate radius for sphere mirror
void GeometricUserParams::calcMirrorRadius(double entranceArmLength,
                                           double exitArmLength) {
    m_radius =
        2.0 / sin(m_alpha) / (1.0 / entranceArmLength + 1.0 / exitArmLength);
}

// calculate radius for sphere grating
void GeometricUserParams::calcGratingRadius(GratingMount mount,
                                            double deviation,
                                            double entranceArmLength,
                                            double exitArmLength) {
    if (mount == GratingMount::Deviation) {
        double theta =
            deviation > 0 ? (PI - deviation) / 2 : PI / 2 + deviation;
        m_radius =
            2.0 / sin(theta) / (1.0 / entranceArmLength + 1.0 / exitArmLength);
    } else if (mount == GratingMount::Incidence) {
        double ca = cos(m_alpha);
        double cb = cos(m_beta);
        m_radius = (ca + cb) /
                   ((ca * ca) / entranceArmLength + (cb * cb) / exitArmLength);
    }
}

// calculate long and short radius for Torus
void GeometricUserParams::calcTorusRadius(double incidenceAngle,
                                          double sEntrance, double sExit,
                                          double mEntrance, double mExit) {
    m_radius = 2.0 / sin(incidenceAngle) / (1.0 / mEntrance + 1.0 / mExit);

    if (mEntrance == 0.0 || mExit == 0.0 || incidenceAngle == 0.0) {
        m_shortRadius = 0.0;
    } else {
        m_shortRadius =
            2.0 * sin(incidenceAngle) / (1.0 / sEntrance + 1.0 / sExit);
    }
}

double GeometricUserParams::getAlpha() { return m_alpha; }

double GeometricUserParams::getBeta() { return m_beta; }

double GeometricUserParams::getRadius() { return m_radius; }

double GeometricUserParams::getShortRadius() { return m_shortRadius; }
}  // namespace RAYX