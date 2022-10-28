#include "ReflectionZonePlate.h"

#include <Material/Material.h>

#include "Debug.h"

namespace RAYX {

ReflectionZonePlate::ReflectionZonePlate(const DesignObject& dobj) : OpticalElement(dobj) {
    m_fresnelZOffset = dobj.parseFresnelZOffset();
    m_designAlphaAngle = dobj.parseDesignAlphaAngle();
    m_designBetaAngle = dobj.parseDesignBetaAngle();
    m_designOrderOfDiffraction = dobj.parseDesignOrderDiffraction();
    m_designEnergy = dobj.parseDesignEnergy();
    m_designSagittalEntranceArmLength = dobj.parseEntranceArmLengthSag();
    m_designSagittalExitArmLength = dobj.parseExitArmLengthSag();
    m_designMeridionalEntranceArmLength = dobj.parseEntranceArmLengthSag();
    m_designMeridionalExitArmLength = dobj.parseExitArmLengthMer();
    m_orderOfDiffraction = dobj.parseOrderDiffraction();

    // m_designEnergy = designEnergy; // if Auto == true, take energy of Source
    // (param sourceEnergy), else m_designEnergy = designEnergy
    m_designWavelength = m_designEnergy == 0 ? 0 : hvlam(m_designEnergy);
    m_additionalOrder = double(dobj.parseAdditionalOrder());

    m_curvatureType = dobj.parseCurvatureType();
    m_designType = DesignType::ZOffset;    // DesignType::ZOffset (0) default
    m_derivationMethod = 0;                // DM_FORMULA default
    m_rzpType = RZPType::Elliptical;       // default (0)
    m_imageType = ImageType::Point2Point;  // default (0)

    Material mat = dobj.parseMaterial();
    auto matd = (double)static_cast<int>(mat);

    // set parameters in Quadric class
    if (m_curvatureType == CurvatureType::Plane) {
        setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 4, 0, matd, 0}));
    } else if (m_curvatureType == CurvatureType::Toroidal) {
        m_longRadius = dobj.parseLongRadius();    // for sphere and toroidal
        m_shortRadius = dobj.parseShortRadius();  // only for Toroidal
        setSurface(std::make_unique<Toroid>(m_longRadius, m_shortRadius, 4, mat));
    } else {
        m_longRadius = dobj.parseLongRadius();  // for sphere and toroidal
        setSurface(std::make_unique<Quadric>(glm::dmat4x4{1, 0, 0, 0, 1, 1, 0, -m_longRadius, 0, 0, 1, 0, 4, 0, matd, 0}));
    }

    printInfo();
    RAYX_VERB << "Created.";
}

void ReflectionZonePlate::printInfo() const {
    std::cout.precision(17);

    if (m_rzpType == RZPType::Elliptical) {
        RAYX_VERB << static_cast<int>(m_rzpType) << ", type: ELLIPTICAL";
    } else if (m_rzpType == RZPType::Meriodional) {
        RAYX_VERB << static_cast<int>(m_rzpType) << ", type: MERIDIONAL";
    } else {
        RAYX_VERB << static_cast<int>(m_rzpType);
    }

    if (m_curvatureType == CurvatureType::Plane) {
        RAYX_VERB << m_designAlphaAngle.rad << ", curvature type: PLANE";
    } else if (m_curvatureType == CurvatureType::Spherical) {
        RAYX_VERB << m_designAlphaAngle.rad << ", curvature type: SPHERICAL";
    } else if (m_curvatureType == CurvatureType::Toroidal) {
        RAYX_VERB << m_designAlphaAngle.rad << ", curvature type: TOROIDAL";
    } else {
        RAYX_VERB << static_cast<int>(m_designType);
    }

    if (m_imageType == ImageType::Point2Point) {
        RAYX_VERB << static_cast<int>(m_imageType) << ", m_imageType: POINT2POINT";
    } else if (m_imageType == ImageType::Astigmatic2Astigmatic) {
        RAYX_VERB << static_cast<int>(m_imageType) << ", m_imageType: ASTIGMATIC2ASTIGMATIC";
    } else {
        RAYX_VERB << static_cast<int>(m_imageType);
    }

    RAYX_VERB << "\t VALUES";
    RAYX_VERB << "\tm_alpha0Angle: " << m_alpha0Angle.rad;
    RAYX_VERB << "\tm_beta0Angle: " << m_beta0Angle.rad;
    RAYX_VERB << "\tm_designAlphaAngle: " << m_designAlphaAngle.rad;
    RAYX_VERB << "\tm_designBetaAngle: " << m_designBetaAngle.rad;
    RAYX_VERB << "\tm_zOff: " << m_zOff;
    RAYX_VERB << "\tm_wavelength: " << m_designWavelength;
    RAYX_VERB << "\tm_lineDensity: " << m_lineDensity;
    RAYX_VERB << "\tm_designOrderOfDiffraction: " << m_designOrderOfDiffraction;
    RAYX_VERB << "\tm_orderOfDiffraction: " << m_orderOfDiffraction;
    RAYX_VERB << "\tm_designEnergyMounting: " << m_designEnergyMounting;
    RAYX_VERB << "\tm_fresnelZOffset: " << m_fresnelZOffset;
    RAYX_VERB << "\tm_shortRadius: " << m_shortRadius;
    RAYX_VERB << "\tm_longRadius: " << m_longRadius;
}

/**
 * needed for caclulating incidence and exit angle
 */
void ReflectionZonePlate::Illumination() {
    double b = m_meridionalDivergence / 1000;
    double a = m_grazingIncidenceAngle.rad * PI / 180;
    double f = 2 * m_meridionalDistance;
    m_illuminationZ = -f * 1 / tan(b) * sin(a) + 1 / sin(b) * sqrt(pow(f, 2) * (pow(cos(b) * sin(a), 2) + pow(sin(b), 2)));
}

/**
 * needed for calculating incidence and exit angle
 */
double ReflectionZonePlate::calcZOffset() {
    Illumination();
    double f = m_meridionalDistance;
    double a = m_grazingIncidenceAngle.rad;
    double IllZ = m_illuminationZ;
    double sq1 = pow(-4 * f * IllZ * cos(a) + 4 * pow(f, 2) + pow(IllZ, 2), -0.5);
    double sq2 = pow(4 * f * IllZ * cos(a) + 4 * pow(f, 2) + pow(IllZ, 2), -0.5);
    double illuminationZoffset;
    if (f == 0 || a == 0) {
        illuminationZoffset = 0;
    } else {
        illuminationZoffset = -(f *
                                pow(1 - pow(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a)), 2) *
                                            pow(pow(fabs(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a))), 2) +
                                                    4 * pow(f, 2) * pow(sq1 + sq2, 2) * pow(sin(a), 2),
                                                -1),
                                    -0.5) *
                                sin(a - acos((-(sq1 * (IllZ - 2 * f * cos(a))) + sq2 * (IllZ + 2 * f * cos(a))) *
                                             pow(pow(fabs(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a))), 2) +
                                                     4 * pow(f, 2) * pow(sq1 + sq2, 2) * pow(sin(a), 2),
                                                 -0.5))));
    }
    return illuminationZoffset;
}

/**
 * needed for calculating exit angle beta
 */
double ReflectionZonePlate::calcDz00() {
    double wavelength = m_designWavelength / 1000 / 1000;  // mm
    // double alpha = m_designAlphaAngle;
    // double beta;
    // double zeta;
    // double fresnelZOffset;
    if (m_designType == DesignType::Beta) {
        // beta = m_designBetaAngle;
        calcFresnelZOffset();  // overwrite given Fresneloffset
    }
    // else { // if(m_designType == DesignType::ZOffset) {
    // beta = m_betaAngle; // what is this beta angle
    // use given fresnelOffset
    //}
    // if imageType == point2pointXstretched ..

    // RAYX-UI calls rzpLineDensity function in fortran
    double DZ = rzpLineDensityDZ(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), wavelength);
    return DZ;
}

/**
 * Calculate fresnel z offset if DesignType == BETA from angle beta.
 * @param:
 */
void ReflectionZonePlate::calcFresnelZOffset() {
    double betaAngle = 0;  // TODO should this really be = 0 if m_designType !=
                           // DesignType::Beta?
    if (m_designType == DesignType::Beta) {
        betaAngle = m_designBetaAngle.rad;
    }
    m_betaAngle = Rad{.rad = betaAngle};
    double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle.rad);
    double ROcosb = m_designSagittalExitArmLength * cos(m_betaAngle.rad);
    double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle.rad);
    double ROsinb = m_designSagittalExitArmLength * sin(m_betaAngle.rad);
    double tanTheta = (RIsina + ROsinb) / (RIcosa + ROcosb);
    m_calcFresnelZOffset = (RIsina / tanTheta) - RIcosa;
}

/**
 * Calculate beta (exit angle) if DesignType == ZOFFSET from fresnel z offset
 * analogous to calcFresnelZOffset if design type is beta.
 */
void ReflectionZonePlate::calcBeta() {
    if (m_designType == DesignType::ZOffset) {
        VectorR2Center();
        if (m_fresnelZOffset != 0) {  // m_fresnelZOffset is given by the user as a parameter bc
                                      // DesignType==DesignType::ZOffset
            m_betaAngle.rad = acos((-m_R2ArmLength * m_R2ArmLength * m_designSagittalExitArmLength * m_designSagittalExitArmLength *
                                    m_fresnelZOffset * m_fresnelZOffset) /
                                   (2 * m_designSagittalExitArmLength * m_fresnelZOffset));
        }
    }
}

/**
 * needed for incidence and exit angle calculation
 */
void ReflectionZonePlate::VectorR1Center() {
    if (m_designType == DesignType::ZOffset) {
        double param_R1cosZ = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle.rad) * m_fresnelZOffset;
        m_R1ArmLength = sqrt(pow(param_R1cosZ, 2) + pow(m_designSagittalEntranceArmLength * sin(m_designAlphaAngle.rad), 2));
        m_alpha0Angle.rad = acos(param_R1cosZ / m_R1ArmLength);
    } else if (m_designType == DesignType::Beta) {
        double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle.rad);
        double ROcosb = m_designSagittalExitArmLength * cos(m_designBetaAngle.rad);
        double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle.rad);
        double ROsinb = m_designSagittalExitArmLength * sin(m_designBetaAngle.rad);
        m_alpha0Angle.rad = (RIsina + ROsinb) / (RIcosa + ROcosb);
        m_R1ArmLength = RIsina / sin(m_alpha0Angle.rad);
    }
}

/**
 * needed for incidence and exit angle calculation
 */
void ReflectionZonePlate::VectorR2Center() {
    if (m_designType == DesignType::ZOffset) {
        VectorR1Center();
        double R2s = m_designSagittalExitArmLength;
        double alpha = m_alpha0Angle.rad;  // why another alpha angle??
        m_R2ArmLength = 0.5 * (-2 * m_fresnelZOffset * cos(alpha) +
                               sqrt(pow(2 * R2s, 2) - 2 * pow(m_fresnelZOffset, 2) + 2 * pow(m_fresnelZOffset, 2) * cos(2 * alpha)));
        m_beta0Angle = m_alpha0Angle;
    } else if (m_designType == DesignType::Beta) {
        double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle.rad);
        double ROcosb = m_designSagittalExitArmLength * cos(m_designBetaAngle.rad);
        double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle.rad);
        double ROsinb = m_designSagittalExitArmLength * sin(m_designBetaAngle.rad);
        m_beta0Angle = Rad{.rad = (RIsina + ROsinb) / (RIcosa + ROcosb)};
        m_R2ArmLength = ROsinb / sin(m_beta0Angle.rad);
    }
}

/**
 * Calculates the design order of diffraction based on the design type (using
 * the design beta angle or the fresnel center offset)
 *
 * @param designOrderOfDiffraction parameter given by user
 */
void ReflectionZonePlate::calcDesignOrderOfDiffraction(const double designOrderOfDiffraction) {
    int presign = 0;  // TODO should this really be = 0 if m_DesignType is
                      // neither DesignType::ZOffset nor DesignType::Beta?
    if (m_designType == DesignType::ZOffset) {
        presign = (m_designAlphaAngle.rad >= m_designBetaAngle.rad) ? -1 : 1;
    } else if (m_designType == DesignType::Beta) {
        presign = (m_fresnelZOffset >= 0) ? -1 : 1;
    }
    RAYX_VERB << "presign: " << presign;
    m_designOrderOfDiffraction = abs(designOrderOfDiffraction) * presign;
}  // design order of diffraction is directly given by the user

/**
 * @brief Needed for calculating incidence and exit angle.
 * Calculates the line density at one specific point (x,y,z) for a given normal
 * at this point and a wavelength
 * @param intersection          coordinates of intersection point
 * @param normal                normal at intersection point
 * @param WL                    wavelength of ray
 * @return line density on RZP in Z direction for given conditions
 */
double ReflectionZonePlate::rzpLineDensityDZ(glm::dvec3 intersection, glm::dvec3 normal, const double WL) {
    double s_beta = sin(m_designAlphaAngle.rad);
    double c_beta = cos(m_designBetaAngle.rad);
    double s_alpha = sin(m_designAlphaAngle.rad);
    double c_alpha = cos(m_designBetaAngle.rad);

    double risag = m_designSagittalEntranceArmLength;
    double rosag = m_designSagittalExitArmLength;
    double rimer = m_designMeridionalEntranceArmLength;
    double romer = m_designMeridionalExitArmLength;

    double DZ;  //, DX;

    // all of the upcoming variables will be defined in each of the following
    // if-else blocks!
    double xi = 0;
    double yi = 0;
    double zi = 0;
    double xm = 0;
    double ym = 0;
    double zm = 0;

    if (m_imageType == ImageType::Point2Point) {  // point to point (standard)
        if (normal.x == 0 && normal.z == 0) {     // plane
            zi = -(risag * c_alpha + intersection.z);
            xi = intersection.x;
            yi = risag * s_alpha;
            zm = rosag * c_beta - intersection.z;
            xm = intersection.x;
            ym = rosag * s_beta;
        } else {  // more general case, can be reduced to the plane with normal
                  // = (0,1,0) and y = 0
            zi = normal.x * normal.z * intersection.x - (normal.x * normal.x + normal.y * normal.y) * (intersection.z + risag * c_alpha) +
                 normal.y * normal.z * (intersection.y - risag * s_alpha);
            xi = (normal.y * intersection.x - normal.x * intersection.y + normal.x * risag * s_alpha);
            yi = -(normal.x * intersection.x) - normal.y * intersection.y - normal.z * intersection.z - normal.z * risag * c_alpha +
                 normal.y * risag * s_alpha;
            zm = normal.x * normal.z * intersection.x + (normal.x * normal.x + normal.y * normal.y) * (-intersection.z + rosag * c_beta) +
                 normal.y * normal.z * (intersection.y - rosag * s_beta);
            xm = (normal.y * intersection.x - normal.x * intersection.y + normal.x * rosag * s_beta);
            ym = -(normal.x * intersection.x) - normal.y * intersection.y - normal.z * intersection.z + normal.z * rosag * c_beta +
                 normal.y * rosag * s_beta;
        }
    } else if (m_imageType == ImageType::Astigmatic2Astigmatic) {  // astigmatic to astigmatix
        double s_rim = rimer < 0 ? -1 : 1;
        double s_rom = romer < 0 ? -1 : 1;
        double c_2alpha = cos(2 * m_designAlphaAngle.rad);
        double c_2beta = cos(2 * m_designBetaAngle.rad);
        if (normal.x == 0 && normal.z == 0) {  //   !plane

            zi = s_rim * (rimer * c_alpha + intersection.z);
            xi = (s_rim * intersection.x * (c_alpha * intersection.z - 2 * s_alpha * s_alpha * rimer + s_alpha * intersection.y + rimer)) /
                 (c_alpha * intersection.z - 2 * s_alpha * s_alpha * risag + s_alpha * intersection.y + risag);
            yi = s_rim * (-rimer * s_alpha + intersection.y);
            zm = s_rom * (romer * c_beta - intersection.z);
            xm = (s_rom * intersection.x * (-c_beta * intersection.z - 2 * s_beta * s_beta * romer + s_beta * intersection.y + romer)) /
                 (c_beta * intersection.z + 2 * s_beta * s_beta * rosag - s_beta * intersection.y - rosag);
            ym = s_rom * (romer * s_beta - intersection.y);
        } else {
            double denominator = intersection.z * c_alpha + risag * c_2alpha + intersection.y * s_alpha;
            double nominator = intersection.x * (intersection.z * c_alpha + rimer * c_2alpha + intersection.y * s_alpha);
            zi = s_rim * ((normal.x * normal.x + normal.y * normal.y) * (intersection.z + rimer * c_alpha) -
                          normal.y * normal.z * (intersection.y - rimer * s_alpha) - (normal.x * normal.z * nominator) / denominator);
            xi = s_rim * (-(normal.x * intersection.y) + normal.x * rimer * s_alpha + (normal.y * nominator) / denominator);
            yi = s_rim * (normal.z * (intersection.z + rimer * c_alpha) + normal.y * (intersection.y - rimer * s_alpha) +
                          (normal.x * nominator) / denominator);

            denominator = (-(intersection.z * c_beta) + rosag * c_2beta + intersection.y * s_beta);
            nominator = intersection.x * (-(intersection.z * c_beta) + romer * c_2beta + intersection.y * s_beta);
            zm = s_rom * ((normal.x * normal.x + normal.y * normal.y) * (-intersection.z + romer * c_beta) +
                          normal.y * normal.z * (intersection.y - romer * s_beta) + (normal.x * normal.z * nominator) / denominator);
            xm = s_rom * (normal.x * (intersection.y - romer * s_beta) - (normal.y * nominator) / denominator);
            ym = s_rom * (normal.z * (-intersection.z + romer * c_beta) + normal.y * (-intersection.y + romer * s_beta) -
                          (normal.x * nominator) / denominator);
        }
        double ris = sqrt(zi * zi + xi * xi + yi * yi);
        double rms = sqrt(zm * zm + xm * xm + ym * ym);

        double ai = zi / ris;
        // double bi = -xi / ris;
        double am = -zm / rms;
        // double bm = xm / rms;

        // double ci = yi/ris; // for what?
        // double cm = -ym/rms;

        DZ = (ai + am) / (WL * m_designOrderOfDiffraction);
        // DX = (-bi - bm) / (WL * m_designOrderOfDiffraction);

        return DZ;
    }

    double ris = sqrt(zi * zi + xi * xi + yi * yi);
    double rms = sqrt(zm * zm + xm * xm + ym * ym);

    // double ai = xi / ris;
    // double am = xm / rms;
    double bi = zi / ris;
    double bm = zm / rms;

    // DX = (ai + am) / (WL * m_designOrderOfDiffraction);
    DZ = (-bi - bm) / (WL * m_designOrderOfDiffraction);

    return DZ;
}

Rad ReflectionZonePlate::getDesignAlphaAngle() const { return m_designAlphaAngle; }
Rad ReflectionZonePlate::getDesignBetaAngle() const { return m_designBetaAngle; }

GratingMount ReflectionZonePlate::getGratingMount() const { return m_gratingMount; }

double ReflectionZonePlate::getLongRadius() const { return m_longRadius; }

double ReflectionZonePlate::getShortRadius() const { return m_shortRadius; }

double ReflectionZonePlate::getFresnelZOffset() const { return m_fresnelZOffset; }

double ReflectionZonePlate::getCalcFresnelZOffset() const {
    return m_calcFresnelZOffset;  // calculated if DesignType==DesignType::Beta
}

// input and exit vector lengths
double ReflectionZonePlate::getSagittalEntranceArmLength() const { return m_designSagittalEntranceArmLength; }

double ReflectionZonePlate::getSagittalExitArmLength() const { return m_designSagittalExitArmLength; }

double ReflectionZonePlate::getMeridionalEntranceArmLength() const { return m_designMeridionalEntranceArmLength; }

double ReflectionZonePlate::getMeridionalExitArmLength() const { return m_designMeridionalExitArmLength; }

double ReflectionZonePlate::getR1ArmLength() const { return m_R1ArmLength; }

double ReflectionZonePlate::getR2ArmLength() const { return m_R2ArmLength; }

double ReflectionZonePlate::getWaveLength() const { return m_designWavelength; }
double ReflectionZonePlate::getDesignEnergy() const { return m_designEnergy; }

double ReflectionZonePlate::getOrderOfDiffraction() const { return m_orderOfDiffraction; }

double ReflectionZonePlate::getDesignOrderOfDiffraction() const { return m_designOrderOfDiffraction; }

double ReflectionZonePlate::getDesignEnergyMounting() const {
    return m_designEnergyMounting;  // derived from source?
}

glm::dmat4x4 ReflectionZonePlate::getElementParameters() const {
    return {double(m_imageType),
            double(m_rzpType),
            double(m_derivationMethod),
            m_designWavelength,
            double(m_curvatureType),
            m_designOrderOfDiffraction,
            m_orderOfDiffraction,
            m_fresnelZOffset,
            m_designSagittalEntranceArmLength,
            m_designSagittalExitArmLength,
            m_designMeridionalEntranceArmLength,
            m_designMeridionalExitArmLength,
            m_designAlphaAngle.rad,
            m_designBetaAngle.rad,
            0,
            double(m_additionalOrder)};
}

}  // namespace RAYX
