#include "CutoutFns.h"

#include "Throw.h"

namespace RAYX {

// checks whether the point (x, z) is within the cutout.
RAYX_FN_ACC
bool RAYX_API inCutout(Cutout cutout, double x, double z) {
    if (cutout.m_type == CTYPE_UNLIMITED) {
        return true;
    } else if (cutout.m_type == CTYPE_RECT) {
        RectCutout rect = deserializeRect(cutout);
        double x_min = -rect.m_width / 2.0;
        double x_max = rect.m_width / 2.0;
        double z_min = -rect.m_length / 2.0;
        double z_max = rect.m_length / 2.0;

        return !(x <= x_min || x >= x_max || z <= z_min || z >= z_max);
    } else if (cutout.m_type == CTYPE_TRAPEZOID) {
        TrapezoidCutout t = deserializeTrapezoid(cutout);

        // Check point is within the trapezoid
        dvec2 P = dvec2(x, z);

        // A, B, C, D are the four points on the trapezoid.
        //
        //    A--B    //
        //   /    \   //
        //  C------D  //
        dvec2 A = dvec2(-t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 B = dvec2(t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 C = dvec2(t.m_widthB / 2.0, t.m_length / 2.0);
        dvec2 D = dvec2(-t.m_widthB / 2.0, t.m_length / 2.0);

        dvec2 PmA = P - A;
        dvec2 BmA = B - A;
        dvec2 PmD = P - D;
        dvec2 CmD = C - D;
        dvec2 DmA = D - A;
        dvec2 PmB = P - B;
        dvec2 CmB = C - B;

        double l1 = (PmA.x * BmA.y - PmA.y * BmA.x) * (PmD.x * CmD.y - PmD.y * CmD.x);
        double l2 = (PmA.x * DmA.y - PmA.y * DmA.x) * (PmB.x * CmB.y - PmB.y * CmB.x);
        return l1 < 0 && l2 < 0;
    } else if (cutout.m_type == CTYPE_ELLIPTICAL) {
        EllipticalCutout ell = deserializeElliptical(cutout);

        double radius_x = ell.m_diameter_x / 2.0;
        double radius_z = ell.m_diameter_z / 2.0;

        double val1 = x / radius_x;
        double val2 = z / radius_z;

        double rd2 = val1 * val1 + val2 * val2;
        return rd2 <= 1.0;
    } else {
        _throw("invalid cutout type in inCutout!");
        return false;
    }
}

// returns a matrix M where (M[i].x, M[i].z) are the key points of our cutout.
// The key points are typically points on the boundary of the cutout.
RAYX_FN_ACC
dmat4 RAYX_API keyCutoutPoints(Cutout cutout) {
    dmat4 ret;
    double w = 0;
    double l = 0;
    if (cutout.m_type == CTYPE_UNLIMITED) {
        double inf = 1e100;
        w = inf;
        l = inf;
    } else if (cutout.m_type == CTYPE_RECT) {
        RectCutout rect = deserializeRect(cutout);
        w = rect.m_width / 2.0;
        l = rect.m_length / 2.0;
    } else if (cutout.m_type == CTYPE_TRAPEZOID) {
        TrapezoidCutout t = deserializeTrapezoid(cutout);

        dvec2 A = dvec2(-t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 B = dvec2(t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 C = dvec2(t.m_widthB / 2.0, t.m_length / 2.0);
        dvec2 D = dvec2(-t.m_widthB / 2.0, t.m_length / 2.0);

        ret[0] = dvec4(A[0], 0.0, A[1], 0.0);
        ret[1] = dvec4(B[0], 0.0, B[1], 0.0);
        ret[2] = dvec4(C[0], 0.0, C[1], 0.0);
        ret[3] = dvec4(D[0], 0.0, D[1], 0.0);
        return ret;
    } else if (cutout.m_type == CTYPE_ELLIPTICAL) {
        EllipticalCutout ell = deserializeElliptical(cutout);
        double rx = ell.m_diameter_x / 2.0;
        double rz = ell.m_diameter_z / 2.0;
        ret[0] = dvec4(rx, 0.0, 0.0, 0.0);
        ret[1] = dvec4(0.0, 0.0, rz, 0.0);
        ret[2] = dvec4(-rx, 0.0, 0.0, 0.0);
        ret[3] = dvec4(0.0, 0.0, -rz, 0.0);
        return ret;
    } else {
        _throw("invalid cutout type in inCutout!");
    }
    ret[0] = dvec4(w, 0.0, l, 0.0);
    ret[1] = dvec4(-w, 0.0, -l, 0.0);
    ret[2] = dvec4(-w, 0.0, l, 0.0);
    ret[3] = dvec4(w, 0.0, -l, 0.0);
    return ret;
}

// returns width and length of the bounding box.
RAYX_FN_ACC
dvec2 RAYX_API cutoutBoundingBox(Cutout cutout) {
    dvec2 ret = dvec2(0.0, 0.0);
    dmat4 keypoints = keyCutoutPoints(cutout);
    for (int i = 0; i < 4; i++) {
        double x = abs(keypoints[i][0]) * 2.0;
        double z = abs(keypoints[i][2]) * 2.0;
        if (x > ret[0]) ret[0] = x;
        if (z > ret[1]) ret[1] = z;
    }
    return ret;
}

// checks whether c1 is a subset of c2, and prints an error otherwise.
// might not find all subset-violations, but should find most of them.
// (might not find all Ellipsoid vs Trapezoid violations)
RAYX_FN_ACC
void RAYX_API assertCutoutSubset(Cutout c1, Cutout c2) {
    dmat4 keypoints = keyCutoutPoints(c1);
    for (int i = 0; i < 4; i++) {
        double x = keypoints[i][0];
        double z = keypoints[i][2];
        if (!inCutout(c2, x, z)) {
            _throw("assertCutoutSubset failed!");
        }
    }
}

}  // namespace RAYX
