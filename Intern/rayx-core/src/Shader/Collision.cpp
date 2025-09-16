#define COLLISION_EPSILON 1e-6

#include "Collision.h"

#include "ApplySlopeError.h"
#include "Cubic.h"
#include "CutoutFns.h"
#include "InvocationState.h"
#include "Throw.h"
#include "Utils.h"

namespace RAYX {

/**************************************************************
 *                    Quadric collision
 **************************************************************/
RAYX_FN_ACC
OptCollisionPoint getQuadricCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection, QuadricSurface q) {
    CollisionPoint col;
    col.hitpoint = glm::dvec3(0, 0, 0);
    col.normal   = glm::dvec3(0, 0, 0);

    int cs     = 1;
    int d_sign = q.m_icurv;
    if (glm::abs(rayDirection[1]) >= glm::abs(rayDirection[0]) && glm::abs(rayDirection[1]) >= glm::abs(rayDirection[2])) {
        cs = 2;
    } else if (glm::abs(rayDirection[2]) >= glm::abs(rayDirection[0]) && glm::abs(rayDirection[2]) >= glm::abs(rayDirection[1])) {
        cs = 3;
    }

    double x = 0;
    double y = 0;
    double z = 0;
    double a = 0;
    double b = 0;
    double c = 0;

    if (cs == 1) {
        double aml = rayDirection[1] / rayDirection[0];
        double anl = rayDirection[2] / rayDirection[0];
        y          = rayPosition[1] - aml * rayPosition[0];
        z          = rayPosition[2] - anl * rayPosition[0];
        d_sign     = int(glm::sign(rayDirection[0]) * q.m_icurv);

        a = q.m_a11 + 2 * q.m_a12 * aml + q.m_a22 * aml * aml + 2 * q.m_a13 * anl + 2 * q.m_a23 * aml * anl + q.m_a33 * anl * anl;
        b = q.m_a14 + q.m_a24 * aml + q.m_a34 * anl + (q.m_a12 + q.m_a22 * aml + q.m_a23 * anl) * y + (q.m_a13 + q.m_a23 * aml + q.m_a33 * anl) * z;
        c = q.m_a44 + q.m_a22 * y * y + 2 * q.m_a34 * z + q.m_a33 * z * z + 2 * y * (q.m_a24 + q.m_a23 * z);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            return std::nullopt;
        } else {
            if (glm::abs(a) > glm::abs(c) * 1e-10) {
                x = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                x = (-c / 2) / b;
            }
        }
        y = y + aml * x;
        z = z + anl * x;
    } else if (cs == 2) {
        double alm = rayDirection[0] / rayDirection[1];
        double anm = rayDirection[2] / rayDirection[1];
        x          = rayPosition[0] - alm * rayPosition[1];
        z          = rayPosition[2] - anm * rayPosition[1];
        d_sign     = int(glm::sign(rayDirection[1]) * q.m_icurv);

        a = q.m_a22 + 2 * q.m_a12 * alm + q.m_a11 * alm * alm + 2 * q.m_a23 * anm + 2 * q.m_a13 * alm * anm + q.m_a33 * anm * anm;
        b = q.m_a24 + q.m_a14 * alm + q.m_a34 * anm + (q.m_a12 + q.m_a11 * alm + q.m_a13 * anm) * x + (q.m_a23 + q.m_a13 * alm + q.m_a33 * anm) * z;
        c = q.m_a44 + q.m_a11 * x * x + 2 * q.m_a34 * z + q.m_a33 * z * z + 2 * x * (q.m_a14 + q.m_a13 * z);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            return std::nullopt;
        } else {
            if (glm::abs(a) > glm::abs(c) * 1e-10) {
                y = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                y = (-c / 2) / b;
            }
        }
        x = x + alm * y;
        z = z + anm * y;
    } else {
        double aln = rayDirection[0] / rayDirection[2];
        double amn = rayDirection[1] / rayDirection[2];
        // firstParam = aln;
        // secondParam = amn;
        x      = rayPosition[0] - aln * rayPosition[2];
        y      = rayPosition[1] - amn * rayPosition[2];
        d_sign = int(glm::sign(rayDirection[2]) * q.m_icurv);

        a = q.m_a33 + 2 * q.m_a13 * aln + q.m_a11 * aln * aln + 2 * q.m_a23 * amn + 2 * q.m_a12 * aln * amn + q.m_a22 * amn * amn;
        b = q.m_a34 + q.m_a14 * aln + q.m_a24 * amn + (q.m_a13 + q.m_a11 * aln + q.m_a12 * amn) * x + (q.m_a23 + q.m_a12 * aln + q.m_a22 * amn) * y;
        c = q.m_a44 + q.m_a11 * x * x + 2 * q.m_a24 * y + q.m_a22 * y * y + 2 * x * (q.m_a14 + q.m_a12 * y);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            return std::nullopt;
        } else {
            if (glm::abs(a) > glm::abs(c) * 1e-10) {  // pow(10, double(-10))) {
                z = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                z = (-c / 2) / b;
            }
        }
        x = x + aln * z;
        y = y + amn * z;
        // rayPosition = glm::dvec3(a, b, c);
    }

    // intersection point is in the negative direction (behind the position when the direction is followed forwards), set weight to 0
    if ((x - rayPosition.x) / rayDirection.x < 0 || (y - rayPosition.y) / rayDirection.y < 0 || (z - rayPosition.z) / rayDirection.z < 0) {
        return std::nullopt;
    }

    col.hitpoint = glm::dvec3(x, y, z);

    double fx  = 2 * q.m_a14 + 2 * q.m_a11 * x + 2 * q.m_a12 * y + 2 * q.m_a13 * z;
    double fy  = 2 * q.m_a24 + 2 * q.m_a12 * x + 2 * q.m_a22 * y + 2 * q.m_a23 * z;
    double fz  = 2 * q.m_a34 + 2 * q.m_a13 * x + 2 * q.m_a23 * y + 2 * q.m_a33 * z;
    col.normal = normalize(glm::dvec3(fx, fy, fz));
    return col;
}

/**************************************************************
 *                    Cubic collision
 **************************************************************/
/**
 * taken from RAY-UI Nov. 2023:
 *  new version of Th. Zeschke by Oct. 18, 2007
 *  corresponding exe-version is called ray_thomas1.exe (on VAX only)
 *
 *  calculates intersection of a straight line
 *  equation straight line:
 *  (x-x1)/al = (y-y1)/am = (z-z1)/an
 *  with a surface of 3. order
 *
 *  method: the iterative Newtonmethod is used for zero point search
 *  result is X,Y,Z of intersection
 * Ray in in element koordinates.
 */
RAYX_FN_ACC
OptCollisionPoint getCubicCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection, CubicSurface cu) {
    // TODO: what is this and do we need it?
    // Ray r = rotateForCubic(rin, cu.m_psi, 1000);

    int cs = 1;
    if (glm::abs(rayDirection[1]) >= glm::abs(rayDirection[0]) && glm::abs(rayDirection[1]) >= glm::abs(rayDirection[2])) {
        cs = 2;
    } else if (glm::abs(rayDirection[2]) >= glm::abs(rayDirection[0]) && glm::abs(rayDirection[2]) >= glm::abs(rayDirection[1])) {
        cs = 3;
    }

    glm::dvec3 pos = cubicPosition(rayPosition, cu.m_psi);
    double x       = pos.x;
    double y       = pos.y;
    double z       = pos.z;
    double x1      = 0;
    double xx      = 0;
    double y1      = 0;
    double yy      = 0;
    double z1      = 0;
    double zz      = 0;
    double counter = 0;
    double dx      = 0;

    glm::dvec3 dir = cubicDirection(rayDirection, cu.m_psi);
    double al      = dir.x;
    double am      = dir.y;
    double an      = dir.z;

    if (cs == 1) {
        double aml = am / al;
        double anl = an / al;

        do {
            x1 = xx;
            y1 = y - aml * (x - xx);
            z1 = z - anl * (x - xx);

            double func = (2 * ((x1 - xx) * an - al * z1) * cu.m_a23 - (2 * cu.m_a24 + cu.m_b12 * pow(float(xx), 2.0)) * al +
                           ((x1 - xx) * am - al * y1) * (cu.m_a22 + cu.m_b21 * xx)) *
                          ((x1 - xx) * am - al * y1);
            func = func + pow(float(((x1 - xx) * an - al * z1)), 2.0) * cu.m_a33;
            func = func - ((x1 - xx) * an - al * z1) * (2 * cu.m_a34 + cu.m_b13 * pow(float(xx), 2.0) * al + cu.m_a44 * pow(float(al), 2.0));
            func = (func -
                    (2 * ((x1 - xx) * an - al * z1) * cu.m_a13 - (cu.m_a11 * xx + 2 * cu.m_a14) * al + 2 * ((x1 - xx) * am - al * y1) * cu.m_a12) *
                        al * xx) *
                   al;
            func = (func - (pow(float(((x1 - xx) * am - al * y1)), 2.0) * cu.m_b23 +
                            ((x1 - xx) * am - al * y1) * ((x1 - xx) * an - al * z1) * cu.m_b32 - ((x1 - xx) * an - al * z1) * al * cu.m_b31 * xx) *
                               ((x1 - xx) * an - al * z1) / pow(float(al), 3));

            double dfunc = (2 * ((x1 - xx) * an - al * z1) * cu.m_a23 - (2 * cu.m_a24 + cu.m_b12 * pow(float(xx), 2)) * al +
                            ((x1 - xx) * am - al * y1) * (cu.m_a22 + cu.m_b21 * xx)) *
                           am;
            dfunc = dfunc - (2 * (cu.m_a12 * am + cu.m_a13 * an) + cu.m_a11 * al) * al * xx;
            dfunc =
                dfunc + (2 * ((x1 - xx) * an - al * z1) * cu.m_a13 - (cu.m_a11 * xx + 2 * cu.m_a14) * al + 2 * ((x1 - xx) * am - al * y1) * cu.m_a12);
            dfunc =
                dfunc * al + ((cu.m_a22 + cu.m_b21 * xx) * am + 2 * (cu.m_a23 * an + al * cu.m_b12 * xx) - ((x1 - xx) * am - al * y1) * cu.m_b21) *
                                 ((x1 - xx) * am - al * y1);
            dfunc = (dfunc + 2 * ((x1 - xx) * an - al * z1) * (cu.m_a33 * an + al * cu.m_b13 * xx) -
                     (2 * cu.m_a34 + cu.m_b13 * pow(float(xx), 2)) * al * an) *
                    al;
            dfunc = (dfunc - ((((x1 - xx) * an - al * z1) * (al * cu.m_b31 + am * cu.m_b32) - al * an * cu.m_b31 * xx +
                               ((x1 - xx) * am - al * y1) * (2 * am * cu.m_b23 + an * cu.m_b32)) *
                                  ((x1 - xx) * an - al * z1) +
                              (pow(float(((x1 - xx) * am - al * y1)), 2) * cu.m_b23 +
                               ((x1 - xx) * am - al * y1) * ((x1 - xx) * an - al * z1) * cu.m_b32 - ((x1 - xx) * an - al * z1) * al * cu.m_b31 * xx) *
                                  an));
            dfunc = dfunc / pow(float(al), 3);

            if (glm::abs(dfunc) < 0.001) { dfunc = 0.001; }

            dx = func / dfunc;
            xx = xx - dx;

            x = x1;
            y = y1;
            z = z1;

            if (counter > 1000) {
                x = -2 * y1 / 2 / aml;
                y = y1 + aml * x;
                z = z1 + anl * x;
            }
            counter++;
        } while (glm::abs(dx) > 0.001);
    } else if (cs == 2) {
        double alm = al / am;
        double anm = an / am;

        do {
            x1 = x - alm * (y - yy);
            y1 = yy;
            z1 = z - anm * (y - yy);

            double func = (2 * (((y1 - yy) * an - am * z1) * cu.m_a13 - (cu.m_a12 * yy + cu.m_a14) * am) +
                           ((y1 - yy) * al - am * x1) * (cu.m_a11 + cu.m_b12 * yy)) *
                          ((y1 - yy) * al - am * x1);
            func = func + (((y1 - yy) * an - am * z1) * cu.m_a33 - 2 * (cu.m_a23 * yy + cu.m_a34) * am) * ((y1 - yy) * an - am * z1) +
                   (2 * cu.m_a24 * yy + cu.m_a44 + cu.m_a22 * pow(float(yy), 2) * pow(float(am), 2));
            func = func * am +
                   ((((y1 - yy) * an - am * z1) * cu.m_b32 - am * cu.m_b23 * yy) * am * yy - pow(float((y1 - yy) * al - am * x1), 2) * cu.m_b13) *
                       ((y1 - yy) * an - am * z1);
            func = func - (pow(float(((y1 - yy) * an - am * z1)), 2) * cu.m_b31 + pow(float(am), 2) * cu.m_b21 * pow(float(yy), 2)) *
                              ((y1 - yy) * al - am * x1);
            func = func / pow(float(am), 3);

            double dfunc = (pow(float((y1 - yy) * an - am * z1), 2) * cu.m_b31 + pow(float(am), 2) * cu.m_b21 * pow(float(yy), 2) * al +
                            2 * (((y1 - yy) * an - am * z1) * an * cu.m_b31 - pow(float(am), 2)) * cu.m_b21 * yy) *
                           ((y1 - yy) * al - am * x1);
            dfunc =
                dfunc -
                ((((y1 - yy) * an - am * z1) * cu.m_b32 - am * cu.m_b23 * yy) * am * yy - pow(float((y1 - yy) * al - am * x1), 2) * cu.m_b13) * an;
            dfunc = dfunc + (2 * ((y1 - yy) * al - am * x1) * al * cu.m_b13 - (am * cu.m_b23 + an * cu.m_b32) * am * yy +
                             (((y1 - yy) * an - am * z1) * cu.m_b32 - am * cu.m_b23 * yy) * am) *
                                ((y1 - yy) * an - am * z1);
            dfunc = dfunc - (((cu.m_a11 + cu.m_b12 * yy) * al + 2 * (cu.m_a12 * am + cu.m_a13 * an) - ((y1 - yy) * al - am * x1) * cu.m_b12) *
                                 ((y1 - yy) * al - am * x1) -
                             2 * (cu.m_a22 * pow(float(am), 2) * yy + cu.m_a23 * pow(float(am), 2) * z1 - cu.m_a23 * am * an * y1 +
                                  2 * cu.m_a23 * am * an * yy + cu.m_a24 * pow(float(am), 2) + cu.m_a33 * am * an * z1 -
                                  cu.m_a33 * pow(float(an), 2) * y1 + cu.m_a33 * pow(float(an), 2) * yy + cu.m_a34 * am * an) +
                             (2 * (((y1 - yy) * an - am * z1) * cu.m_a13 - (cu.m_a12 * yy + cu.m_a14) * am) +
                              ((y1 - yy) * al - am * x1) * (cu.m_a11 + cu.m_b12 * yy)) *
                                 al) *
                                am;
            dfunc = dfunc / pow(float(am), 3);

            if (glm::abs(dfunc) < 0.001) { dfunc = 0.001; }

            dx = func / dfunc;
            yy = yy - dx;

            x = x1;
            y = y1;
            z = z1;

            if (counter > 1000) {
                x = x1;
                y = 0;
                z = z1;
            }
            counter++;
        } while (glm::abs(dx) > 0.001);
    } else {
        double aln = al / an;
        double amn = am / an;

        do {
            x1 = x - aln * (z - zz);
            y1 = y - amn * (z - zz);
            z1 = zz;

            double func = ((2 * (((z1 - zz) * am - an * y1) * cu.m_a12 - (cu.m_a13 * zz + cu.m_a14) * an) + ((z1 - zz) * al - an * x1) * cu.m_a11) *
                               ((z1 - zz) * al - an * x1) +
                           (((z1 - zz) * am - an * y1) * cu.m_a22 - 2 * (cu.m_a23 * zz + cu.m_a24) * an) * ((z1 - zz) * am - an * y1) +
                           (2 * cu.m_a34 * zz + cu.m_a44 + cu.m_a33 * pow(float(zz), 2)) * pow(float(an), 2)) *
                          an;
            func = func - ((((z1 - zz) * am - an * y1) * cu.m_b12 - an * cu.m_b13 * zz) * pow(float(((z1 - zz) * al - an * x1)), 2) -
                           (((z1 - zz) * am - an * y1) * cu.m_b23 - an * cu.m_b32 * zz) * ((z1 - zz) * am - an * y1) * an * zz +
                           (pow(float(((z1 - zz) * am - an * y1)), 2) * cu.m_b21 + pow(float(an), 2) * cu.m_b31 * pow(float(zz), 2)) *
                               ((z1 - zz) * al - an * x1));
            func = func / pow(float(an), 3);

            double dfunc = (((z1 - zz) * am - an * y1) * cu.m_a22 - 2 * (cu.m_a23 * zz + cu.m_a24) * an) * am +
                           (2 * (cu.m_a12 * am + cu.m_a13 * an) + cu.m_a11 * al) * ((z1 - zz) * al - an * x1);
            dfunc = dfunc + ((z1 - zz) * am - an * y1) * (cu.m_a22 * am + 2 * cu.m_a23 * an) - 2 * (cu.m_a33 * zz + cu.m_a34) * pow(float(an), 2);
            dfunc = (dfunc +
                     (2 * (((z1 - zz) * am - an * y1) * cu.m_a12 - (cu.m_a13 * zz + cu.m_a14) * an) + ((z1 - zz) * al - an * x1) * cu.m_a11) * al) *
                    an;
            dfunc = dfunc - (2 *
                                 (((z1 - zz) * am - an * y1) * am * cu.m_b21 - pow(float(an), 2) * cu.m_b31 * zz +
                                  (((z1 - zz) * am - an * y1) * cu.m_b12 - an * cu.m_b13 * zz) * al) *
                                 ((z1 - zz) * al - an * x1) +
                             (pow(float(((z1 - zz) * am - an * y1)), 2) * cu.m_b21 + pow(float(an), 2) * cu.m_b31 * pow(float(zz), 2)) * al +
                             pow(float(((z1 - zz) * al - an * x1)), 2) * (am * cu.m_b12 + an * cu.m_b13) -
                             ((z1 - zz) * am - an * y1) * (am * cu.m_b23 + an * cu.m_b32) * an * zz +
                             (((z1 - zz) * am - an * y1) * cu.m_b23 - an * cu.m_b32 * zz) * (am * z1 - 2 * am * zz - an * y1) * an);
            dfunc = (-dfunc) / pow(float(an), 3);

            if (glm::abs(dfunc) < 0.001) { dfunc = 0.001; }

            dx = func / dfunc;
            zz = zz - dx;

            x = x1;
            y = y1;
            z = z1;

            if (counter > 1000) {
                x = x1 + aln * z;
                y = y1 + amn * z;
                z = -2 * y1 / 2 / amn;
            }
            counter++;
        } while (glm::abs(dx) > 0.001);
        // rayPosition = glm::dvec3(a, b, c);
    }

    // intersection point is in the negative direction (behind the position when the direction is followed forwards), set weight to 0
    // if ((x - rayPosition.x) / rayDirection.x < 0 || (y - rayPosition.y) / rayDirection.y < 0 || (z - rayPosition.z) / rayDirection.z < 0) {
    //    col.found = false;
    //}

    double fx = 2 * cu.m_a14 + 2 * cu.m_a11 * x + 2 * cu.m_a12 * y + 2 * cu.m_a13 * z;
    double fy = 2 * cu.m_a24 + 2 * cu.m_a12 * x + 2 * cu.m_a22 * y + 2 * cu.m_a23 * z;
    double fz = 2 * cu.m_a34 + 2 * cu.m_a13 * x + 2 * cu.m_a23 * y + 2 * cu.m_a33 * z;

    const auto hitpoint = glm::dvec3(x, y * glm::cos(-cu.m_psi) - z * glm::sin(-cu.m_psi), z * glm::cos(-cu.m_psi) + y * glm::sin(-cu.m_psi));
    const auto normal =
        normalize(glm::dvec3(fx, fy * glm::cos(-cu.m_psi) - fz * glm::sin(-cu.m_psi), fz * glm::cos(-cu.m_psi) + fy * glm::sin(-cu.m_psi)));

    return CollisionPoint{
        .hitpoint = hitpoint,
        .normal   = normal,
    };
}

/**************************************************************
 *                    Toroid Collision
 **************************************************************/
// this uses newton to approximate a solution.
RAYX_FN_ACC
OptCollisionPoint getToroidCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection, ToroidSurface toroid,
                                     bool isTriangul) {
    // Constants
    const double NEW_TOLERANCE   = 0.0001;
    const int NEW_MAX_ITERATIONS = 50;

    double longRad  = toroid.m_longRadius;
    double shortRad = (toroid.m_toroidType == TOROID_TYPE_CONVEX) ? -toroid.m_shortRadius : toroid.m_shortRadius;

    // sign radius: +1 = concave, -1 = convex
    double isigro = glm::sign(shortRad);

    // double aln = rayDirection.x / rayDirection.z;
    // double amn = rayDirection.y / rayDirection.z;

    glm::dvec4 normal         = glm::dvec4(0, 0, 0, 0);
    double xx                 = 0.0;
    double zz                 = 0.0;
    double yy                 = 0.0;
    double dz                 = 0.0;
    glm::dvec3 normalized_dir = glm::dvec3(rayDirection) / rayDirection.z;

    int n = 0;
    // Newton's method iteration
    // While not converged...
    do {
        zz = zz + dz;
        xx = rayPosition.x + normalized_dir.x * (zz - rayPosition.z);
        if (xx * xx > shortRad * shortRad) { xx = xx / glm::abs(xx) * 0.95 * shortRad; }
        yy        = rayPosition.y + normalized_dir.y * (zz - rayPosition.z);
        double sq = sqrt(shortRad * shortRad - xx * xx);
        double rx = (longRad - shortRad + isigro * sq);

        // Calculate toroid normal
        normal.x = (-2 * xx * isigro / sq) * rx;
        normal.y = -2 * (yy - longRad);
        normal.z = -2 * zz;

        double func = -rx * rx + (yy - longRad) * (yy - longRad) + zz * zz;
        double df   = normalized_dir.x * normal.x + normalized_dir.y * normal.y + normal.z;  // dot(normalized_dir, glm::dvec3(normal));
        dz          = func / df;
        n += 1;
        if (n >= NEW_MAX_ITERATIONS) { return std::nullopt; }
    } while (glm::abs(dz) > NEW_TOLERANCE);

    CollisionPoint col;
    col.normal   = normalize(glm::dvec3(normal));
    col.hitpoint = glm::dvec3(xx, yy, zz);

    if (isTriangul)  // TODO: Hack, Triangulation sensetive to direction apparently. Actual fix or func rework is needed!
        return col;

    glm::dvec3 rayToHitpoint = col.hitpoint - rayPosition;
    // edit: if ray points away from the hitpoint, no collision can be found.
    // Note that multiplying the rays direction with -1 SHOULD totally have an effect on the collision detection - in most cases this 180° rotation
    // will make the ray point away from the toroid, and hence preventing a Collision completely. The above code however, is unaffected when
    // multiplying the ray direction with -1. Due to it having no effect on `glm::dvec3 normalized_dir = glm::dvec3(rayDirection) / rayDirection.z;`
    if (dot(rayToHitpoint, rayDirection) <= 0.0) return std::nullopt;

    return col;
}

RAYX_FN_ACC
OptCollisionPoint getPlaneCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection) {
    // the `time` that it takes for the ray to hit the plane (if we understand the rays direction as its velocity).
    // velocity = distance/time <-> time = distance/velocity from school physics.
    // (We need to negate the position, as with positive velocity, you need a negative position to eventually reach the zero point (aka the
    // plane). Having positive position & positive velocity means that we never hit the plane as we move away from it.)
    double time = -rayPosition.y / rayDirection.y;

    // the ray should not face away from the plane (or equivalently, the ray should not come *from* the plane). If that is the case we set
    // `found = false`.
    if (time < 0) return std::nullopt;

    CollisionPoint col;
    col.normal     = glm::dvec3(0, -glm::sign(rayDirection.y), 0);
    col.hitpoint.x = rayPosition.x + rayDirection.x * time;
    col.hitpoint.z = rayPosition.z + rayDirection.z * time;
    col.hitpoint.y = 0;
    return col;
}

/**************************************************************
 *                    Collision Finder
 **************************************************************/

// TODO: remove parameter isTriangul, which is required by RAUX-UI
RAYX_FN_ACC
OptCollisionPoint RAYX_API findCollisionInElementCoordsWithoutSlopeError(const glm::dvec3& __restrict rayPosition,
                                                                         const glm::dvec3& __restrict rayDirection,
                                                                         const OpticalElement& __restrict element, bool isTriangul) {
    OptCollisionPoint col;
    switch (element.m_surface.m_type) {
        case SurfaceType::Plane:
            col = getPlaneCollision(rayPosition, rayDirection);
            break;
        case SurfaceType::Toroid:
            col = getToroidCollision(rayPosition, rayDirection, deserializeToroid(element.m_surface), isTriangul);
            break;
        case SurfaceType::Quadric:
            col = getQuadricCollision(rayPosition, rayDirection, deserializeQuadric(element.m_surface));
            break;
        case SurfaceType::Cubic:
            col = getCubicCollision(rayPosition, rayDirection, deserializeCubic(element.m_surface));
            break;
        default:
            _throw("invalid surfaceType: %d!", static_cast<int>(element.m_surface.m_type));
            return std::nullopt;
    }

    if (!col) return std::nullopt;

    // cutout is applied in the XZ plane.
    if (!inCutout(element.m_cutout, col->hitpoint.x, col->hitpoint.z)) { return std::nullopt; }

    // Both rayDirection and col.normal are in element coordinates.
    // The collision normal should point 'outward from the surface', meaning it should oppose the ray's direction.
    // In other words, we want `dot(rayDirection, col.normal) <= 0`.
    // The default normal may oppose the concave part of the overall shape
    // Depending on whether the element is hit on a concave or convex surface,
    // we flip the normal to ensure it points against the ray's direction.
    if (dot(rayDirection, col->normal) > 0.0) { col->normal *= -1.0; }
    return col;
}

// checks whether `r` collides with the element of the given `id`,
// and returns a Collision accordingly.
RAYX_FN_ACC
OptCollisionPoint findCollisionInElementCoords(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                               const OpticalElement& __restrict element, Rand& __restrict rand) {
    auto col = findCollisionInElementCoordsWithoutSlopeError(rayPosition, rayDirection, element, false);

    if (!col) return std::nullopt;

    SlopeError sE = element.m_slopeError;
    col->normal   = applySlopeError(col->normal, sE, 0, rand);

    return col;
}

RAYX_FN_ACC
OptCollisionWithElement findCollisionWithElements(glm::dvec3 rayPosition, glm::dvec3 rayDirection, const OpticalElement* __restrict elements,
                                                  const int numElements, Rand& __restrict rand) {
    // global coordinates of first intersection point of ray among all elements in beamline
    OptCollisionPoint best_col = std::nullopt;

    // the distance the ray has to travel to reach `best_col`.
    auto best_dist = std::numeric_limits<double>::max();

    // best element so far
    auto best_element = 0;

    // move ray slightly forward.
    // -> prevents hitting an element very close to the previous collision.
    // -> prevents self-intersection.
    rayPosition += rayDirection * COLLISION_EPSILON;

    // Find intersection point through all elements
    for (int elementIndex = 0; elementIndex < numElements; elementIndex++) {
        const auto& element = elements[elementIndex];

        rayMatrixMult(element.m_inTrans, rayPosition, rayDirection);
        const auto current_col = findCollisionInElementCoords(rayPosition, rayDirection, element, rand);
        if (!current_col) continue;

        // calculate distance from ray start to intersection point. doing in element coordinates is totally fine
        const auto current_dist = glm::length(current_col->hitpoint - rayPosition);

        if (current_dist < best_dist) {
            best_col     = current_col;
            best_dist    = current_dist;
            best_element = elementIndex;
        }
        rayMatrixMult(element.m_outTrans, rayPosition, rayDirection);
    }

    if (!best_col) return std::nullopt;
    return CollisionWithElement{*best_col, best_element};
}

}  // namespace RAYX
