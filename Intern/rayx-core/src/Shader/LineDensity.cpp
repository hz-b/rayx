#include "LineDensity.h"
#include "Approx.h"

/** variable line spacing for gratings
@params
    lineDensity: general line density?
    z: z-coordinate of ray position
    vls[6]: 6 vls parameters given by user
@returns line density specifically for this z-coordinate
*/
double RAYX_API vlsGrating(double lineDensity, dvec3 normal, double z, double vls[6]) {
    // lineDensity = lineDensity * (1 + 2*b2*z + 3*b3*z**2 + 4*b4*z**3 +
    // 5*b5*z**4 + 6*b6*z**5 + 7*b7*z**6)

    double del1 = r8_asin(normal.z);
    double cos_d = r8_cos(-del1); // linedesity is smaller on konvex surfaces

    double z2 = z * z;
    double z3 = z2 * z;
    double z4 = z3 * z;
    double z5 = z4 * z;
    double z6 = z5 * z;
    double a = lineDensity * (1 + 2 * vls[0] * z + 3 * vls[1] * z2 + 4 * vls[2] * z3 + 5 * vls[3] * z4 + 6 * vls[4] * z5 + 7 * vls[5] * z6);
    return a * cos_d;
}


/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@params: lots
@returns: (inplace) DX, DZ
*/
void RAYX_API RZPLineDensity(Ray r, dvec3 normal, RZPBehaviour b, RAYX_INOUT(double) DX, RAYX_INOUT(double) DZ) {
    int IMAGE_TYPE = int(b.m_imageType);
    int RZP_TYPE = int(b.m_rzpType);
    double risag = b.m_designSagittalEntranceArmLength;
    double rosag = b.m_designSagittalExitArmLength;
    double rimer = b.m_designMeridionalEntranceArmLength;
    double romer = b.m_designMeridionalExitArmLength;
    double alpha = b.m_designAlphaAngle;
    double beta = b.m_designBetaAngle;
    double WL = 1e-06 * b.m_designWavelength;  // source energy/design energy
    double Ord = b.m_orderOfDiffraction;

    double FX = normal.x;
    double FY = normal.y;
    double FZ = normal.z;
    double X = r.m_position.x;
    double Y = r.m_position.y;
    double Z = r.m_position.z;

    if (RZP_TYPE == 1)  // meridional (wie VLS grating)
        X = 0;

    // avoid calculating the same sinus/cosinus multiple times (costly)
    double s_beta = r8_sin(beta);
    double c_beta = r8_cos(beta);
    double s_alpha = r8_sin(alpha);
    double c_alpha = r8_cos(alpha);

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
        double s_rim = sign(rimer);
        double s_rom = sign(romer);
        double c_2alpha = r8_cos(2 * alpha);
        double c_2beta = r8_cos(2 * beta);
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
            double nominator = X * (Z * c_alpha + rimer * c_2alpha + Y * s_alpha);
            zi = s_rim * ((FX * FX + FY * FY) * (Z + rimer * c_alpha) - FY * FZ * (Y - rimer * s_alpha) - (FX * FZ * nominator) / denominator);
            xi = s_rim * (-(FX * Y) + FX * rimer * s_alpha + (FY * nominator) / denominator);
            yi = s_rim * (FZ * (Z + rimer * c_alpha) + FY * (Y - rimer * s_alpha) + (FX * nominator) / denominator);

            denominator = (-(Z * c_beta) + rosag * c_2beta + Y * s_beta);
            nominator = X * (-(Z * c_beta) + romer * c_2beta + Y * s_beta);
            zm = s_rom * ((FX * FX + FY * FY) * (-Z + romer * c_beta) + FY * FZ * (Y - romer * s_beta) + (FX * FZ * nominator) / denominator);
            xm = s_rom * (FX * (Y - romer * s_beta) - (FY * nominator) / denominator);
            ym = s_rom * (FZ * (-Z + romer * c_beta) + FY * (-Y + romer * s_beta) - (FX * nominator) / denominator);
        }
        double ris = sqrt(zi * zi + xi * xi + yi * yi);
        double rms = sqrt(zm * zm + xm * xm + ym * ym);

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

    double ris = sqrt(zi * zi + xi * xi + yi * yi);
    double rms = sqrt(zm * zm + xm * xm + ym * ym);

    double ai = xi / ris;
    double am = xm / rms;
    double bi = zi / ris;
    double bm = zm / rms;

    DX = (ai + am) / (WL * Ord);
    DZ = (-bi - bm) / (WL * Ord);

    return;
}
