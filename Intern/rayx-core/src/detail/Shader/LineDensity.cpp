#include "LineDensity.h"

#include "Element/Behaviour.h"
#include "ImageType.h"
#include "Throw.h"

namespace rayx {

/** Computes the adjusted line density at a specific z-coordinate for variable line spacing (VLS) gratings,
 * considering both the surface orientation and polynomial VLS parameters.
 *
 * @param lineDensity The base line density of the grating.
 * @param normal A 3-dimensional vector representing the normal to the grating surface at the point of interest.
 * @param z The z-coordinate at which the line density is to be evaluated.
 * @param vls An array of six coefficients that modify the line density based on a polynomial of z. These coefficients
 *            scale the impact of z raised to powers from 1 to 6.
 *
 * @return The modified line density at the given z-coordinate, adjusted for the grating's surface tilt and the polynomial
 *         specified by the VLS coefficients.
 */
RAYX_FN_ACC
double RAYX_API vlsGrating(double lineDensity, glm::dvec3 normal, double z, const double vls[6]) {
    // Calculate the inclination angle from the vertical based on the z-component of the surface normal.
    double del1 = glm::asin(normal.z);

    // Compute the cosine of the negative inclination angle to adjust line density based on the grating's tilt.
    double cos_d = glm::cos(-del1);  // linedesity increases with konvex surface structure

    // Compute powers of z from z^2 to z^6 for polynomial calculation of line density adjustments.
    double z2 = z * z;
    double z3 = z2 * z;
    double z4 = z3 * z;
    double z5 = z4 * z;
    double z6 = z5 * z;
    // Calculate the modified line density using a polynomial expression, incorporating z and the coefficients from vls.
    double a = lineDensity * (1 + 2 * vls[0] * z + 3 * vls[1] * z2 + 4 * vls[2] * z3 + 5 * vls[3] * z4 + 6 * vls[4] * z5 + 7 * vls[5] * z6);
    // Return the adjusted line density considering both polynomial modifications and the cosine adjustment for tilt.
    return a * cos_d;
}

/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@returns: (inplace) DX, DZ
*/
RAYX_FN_ACC
void RAYX_API RZPLineDensity(const glm::dvec3& __restrict position, const glm::dvec3& __restrict normal, const Behaviour::RZP& __restrict b,
                             double& __restrict DX, double& __restrict DZ) {
    int IMAGE_TYPE = b.m_imageType;
    int RZP_TYPE   = b.m_rzpType;
    double risag   = b.m_designSagittalEntranceArmLength;
    double rosag   = b.m_designSagittalExitArmLength;
    double rimer   = b.m_designMeridionalEntranceArmLength;
    double romer   = b.m_designMeridionalExitArmLength;
    double alpha   = b.m_designAlphaAngle;
    double beta    = b.m_designBetaAngle;
    double WL      = 1e-06 * b.m_designWavelength;  // source energy/design energy
    int Ord        = b.m_orderOfDiffraction;

    double FX = normal.x;
    double FY = normal.y;
    double FZ = normal.z;
    double X  = position.x;
    double Y  = position.y;
    double Z  = position.z;

    if (RZP_TYPE == 1)  // meridional (wie VLS grating)
        X = 0;

    // avoid calculating the same sinus/cosinus multiple times (costly)
    double s_beta  = glm::sin(beta);
    double c_beta  = glm::cos(beta);
    double s_alpha = glm::sin(alpha);
    double c_alpha = glm::cos(alpha);

    /*
    if (DERIVATION_METHOD == 1) {
        DX = getLineDensity1d(ptr_dx,x,z);
        DZ = getLineDensity1d(ptr_dz,x,z);
        return;
    }
    */
    double xi = 0;
    double yi = 0;
    double zi = 0;
    double xm = 0;
    double ym = 0;
    double zm = 0;

    if (IMAGE_TYPE == IT_POINT2POINT) {
        if (FX == 0 && FZ == 0) {  // plane
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = X;
            ym = rosag * s_beta;
        } else {  // more general case, can be reduced to the plane with normal
                  // = (0,1,0) and y = 0
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = (FY * X - FX * Y + FX * risag * s_alpha);
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = FX * FZ * X + (FX * FX + FY * FY) * (-Z + rosag * c_beta) + FY * FZ * (Y - rosag * s_beta);
            xm = (FY * X - FX * Y + FX * rosag * s_beta);
            ym = -(FX * X) - FY * Y - FZ * Z + FZ * rosag * c_beta + FY * rosag * s_beta;
        }
    } else if (IMAGE_TYPE == IT_ASTIGMATIC2ASTIGMATIC) {
        double s_rim    = glm::sign(rimer);
        double s_rom    = glm::sign(romer);
        double c_2alpha = glm::cos(2 * alpha);
        double c_2beta  = glm::cos(2 * beta);
        if (FX == 0 && FZ == 0) {  //   !plane

            zi = s_rim * (rimer * c_alpha + Z);
            xi = (s_rim * X * (c_alpha * Z - 2 * s_alpha * s_alpha * rimer + s_alpha * Y + rimer)) /
                 (c_alpha * Z - 2 * s_alpha * s_alpha * risag + s_alpha * Y + risag);
            yi = s_rim * (-rimer * s_alpha + Y);
            zm = s_rom * (romer * c_beta - Z);
            xm = (s_rom * X * (-c_beta * Z - 2 * s_beta * s_beta * romer + s_beta * Y + romer)) /
                 (c_beta * Z + 2 * s_beta * s_beta * rosag - s_beta * Y - rosag);
            ym = s_rom * (romer * s_beta - Y);
        } else {
            double denominator = Z * c_alpha + risag * c_2alpha + Y * s_alpha;
            double nominator   = X * (Z * c_alpha + rimer * c_2alpha + Y * s_alpha);
            zi = s_rim * ((FX * FX + FY * FY) * (Z + rimer * c_alpha) - FY * FZ * (Y - rimer * s_alpha) - (FX * FZ * nominator) / denominator);
            xi = s_rim * (-(FX * Y) + FX * rimer * s_alpha + (FY * nominator) / denominator);
            yi = s_rim * (FZ * (Z + rimer * c_alpha) + FY * (Y - rimer * s_alpha) + (FX * nominator) / denominator);

            denominator = (-(Z * c_beta) + rosag * c_2beta + Y * s_beta);
            nominator   = X * (-(Z * c_beta) + romer * c_2beta + Y * s_beta);
            zm = s_rom * ((FX * FX + FY * FY) * (-Z + romer * c_beta) + FY * FZ * (Y - romer * s_beta) + (FX * FZ * nominator) / denominator);
            xm = s_rom * (FX * (Y - romer * s_beta) - (FY * nominator) / denominator);
            ym = s_rom * (FZ * (-Z + romer * c_beta) + FY * (-Y + romer * s_beta) - (FX * nominator) / denominator);
        }
        double ris = glm::sqrt(zi * zi + xi * xi + yi * yi);
        double rms = glm::sqrt(zm * zm + xm * xm + ym * ym);

        double ai = zi / ris;
        double bi = -xi / ris;
        double am = -zm / rms;
        double bm = xm / rms;

        // double ci = yi/ris; // for what?
        // double cm = -ym/rms;

        DZ = (ai + am) / (WL * Ord);
        DX = (-bi - bm) / (WL * Ord);

        return;
    } else if (IMAGE_TYPE == IT_POINT2HORIZONTAL_LINE) {
        // TODO don't use magic constants

        if (FX == 0.0 && FZ == 0.0) {  // plane
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = 0;
            ym = rosag * s_beta;
        } else {
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = FY * X - FX * Y + FX * risag * s_alpha;
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = (FX * FX + FY * FY) * (-Z + rosag * c_beta) + FY * FZ * (Y - rosag * s_beta);
            xm = FX * (-Y + rosag * s_beta);
            ym = -(FY * Y) - FZ * Z + FZ * rosag * c_beta + FY * rosag * s_beta;
        }
    } else if (IMAGE_TYPE == IT_POINT2HORIZONTAL_DIVERGENT_LINE) {
        if (FX == 0.0 && FZ == 0.0) {
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = X / zi * zm;  // not anymore negative as zi is negative!g
            ym = rosag * s_beta;
        } else {
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = FY * X - FX * Y + FX * risag * s_alpha;
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = (FX * FZ * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha) + (FX * FX + FY * FY) * (-Z + rosag * c_beta) +
                 FY * FZ * (Y - rosag * s_beta);
            xm = (FY * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha) + FX * (-Y + rosag * s_beta);
            ym = -((FX * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha)) + FZ * (-Z + rosag * c_beta) + FY * (-Y + rosag * s_beta);
        }
    } else {
        _throw("unsupported ImageType!");
    }

    double ris = glm::sqrt(zi * zi + xi * xi + yi * yi);
    double rms = glm::sqrt(zm * zm + xm * xm + ym * ym);

    double ai = xi / ris;
    double am = xm / rms;
    double bi = zi / ris;
    double bm = zm / rms;

    DX = (ai + am) / (WL * Ord);
    DZ = (-bi - bm) / (WL * Ord);
}

}  // namespace rayx
