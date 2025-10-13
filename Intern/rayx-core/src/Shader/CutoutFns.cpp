#include "CutoutFns.h"

#include "Throw.h"
#include "Variant.h"

namespace RAYX {

// checks whether the point (x, z) is within the cutout.
RAYX_FN_ACC
bool RAYX_API inCutout(Cutout cutout, double x, double z) {
    return cutout.visit(
        [&]<typename T>(const T& cutout_type) {
            if constexpr (std::is_same_v<T, Cutout::Unlimited>) {
                return true;
            } else if constexpr (std::is_same_v<T, Cutout::Rect>) {
                double x_min = -cutout_type.m_width / 2.0;
                double x_max = cutout_type.m_width / 2.0;
                double z_min = -cutout_type.m_length / 2.0;
                double z_max = cutout_type.m_length / 2.0;

                return !(x <= x_min || x >= x_max || z <= z_min || z >= z_max);
            } else if constexpr (std::is_same_v<T, Cutout::Trapezoid>) {
                // Check point is within the trapezoid
                auto P = glm::dvec2(x, z);

                // A, B, C, D are the four points on the trapezoid.
                //
                //    A--B    //
                //   /    \   //
                //  C------D  //
                auto A = glm::dvec2(-cutout_type.m_widthA / 2.0, -cutout_type.m_length / 2.0);
                auto B = glm::dvec2(cutout_type.m_widthA / 2.0, -cutout_type.m_length / 2.0);
                auto C = glm::dvec2(cutout_type.m_widthB / 2.0, cutout_type.m_length / 2.0);
                auto D = glm::dvec2(-cutout_type.m_widthB / 2.0, cutout_type.m_length / 2.0);

                glm::dvec2 PmA = P - A;
                glm::dvec2 BmA = B - A;
                glm::dvec2 PmD = P - D;
                glm::dvec2 CmD = C - D;
                glm::dvec2 DmA = D - A;
                glm::dvec2 PmB = P - B;
                glm::dvec2 CmB = C - B;

                double l1 = (PmA.x * BmA.y - PmA.y * BmA.x) * (PmD.x * CmD.y - PmD.y * CmD.x);
                double l2 = (PmA.x * DmA.y - PmA.y * DmA.x) * (PmB.x * CmB.y - PmB.y * CmB.x);
                return l1 < 0 && l2 < 0;
            } else if constexpr (std::is_same_v<T, Cutout::Elliptical>) {
                double radius_x = cutout_type.m_diameter_x / 2.0;
                double radius_z = cutout_type.m_diameter_z / 2.0;
                double val1 = x / radius_x;
                double val2 = z / radius_z;
                double rd2 = val1 * val1 + val2 * val2;
                return rd2 <= 1.0;
            } else {
                _throw("invalid cutout type in inCutout!");
                return false;
            }
        }
        );  // to ensure cutout is valid
}

// returns a matrix M where (M[i].x, M[i].z) are the key points of our cutout.
// The key points are typically points on the boundary of the cutout.
RAYX_FN_ACC
glm::dmat4 RAYX_API keyCutoutPoints(Cutout cutout) {
    return cutout.visit(
        [&]<typename T>(const T& cutout_type) {
            if constexpr (std::is_same_v<T, Cutout::Unlimited>) {
                double inf = 1e100;
                return glm::dmat4(glm::dvec4(inf, 0.0, inf, 0.0),    // Top-right
                                  glm::dvec4(-inf, 0.0, -inf, 0.0),  // Bottom-left
                                  glm::dvec4(-inf, 0.0, inf, 0.0),   // Top-left
                                  glm::dvec4(inf, 0.0, -inf, 0.0)    // Bottom-right
                );
            } else if constexpr (std::is_same_v<T, Cutout::Rect>) {
                double w = cutout_type.m_width / 2.0;
                double l = cutout_type.m_length / 2.0;
                return glm::dmat4(glm::dvec4(w, 0.0, l, 0.0),    // Top-right
                                  glm::dvec4(-w, 0.0, -l, 0.0),  // Bottom-left
                                  glm::dvec4(-w, 0.0, l, 0.0),   // Top-left
                                  glm::dvec4(w, 0.0, -l, 0.0)    // Bottom-right
                );
            } else if constexpr (std::is_same_v<T, Cutout::Trapezoid>) {
                auto A = glm::dvec2(-cutout_type.m_widthA / 2.0, -cutout_type.m_length / 2.0);
                auto B = glm::dvec2(cutout_type.m_widthA / 2.0, -cutout_type.m_length / 2.0);
                auto C = glm::dvec2(cutout_type.m_widthB / 2.0, cutout_type.m_length / 2.0);
                auto D = glm::dvec2(-cutout_type.m_widthB / 2.0, cutout_type.m_length / 2.0);

                return glm::dmat4(glm::dvec4(B[0], 0.0, B[1], 0.0),  // Top-right
                                  glm::dvec4(A[0], 0.0, A[1], 0.0),  // Bottom-left
                                  glm::dvec4(D[0], 0.0, D[1], 0.0),  // Top-left
                                  glm::dvec4(C[0], 0.0, C[1], 0.0)   // Bottom-right
                );
            } else if constexpr (std::is_same_v<T, Cutout::Elliptical>) {
                double rx = cutout_type.m_diameter_x / 2.0;
                double rz = cutout_type.m_diameter_z / 2.0;
                return glm::dmat4(glm::dvec4(rx, 0.0, 0.0, 0.0),   // Right
                                  glm::dvec4(0.0, 0.0, rz, 0.0),   // Top
                                  glm::dvec4(-rx, 0.0, 0.0, 0.0),  // Left
                                  glm::dvec4(0.0, 0.0, -rz, 0.0)   // Bottom
                );
            } else {
                _throw("invalid cutout type in keyCutoutPoints!");
                return glm::dmat4(0.0);
            }
        });  // to ensure cutout is valid
}

// returns width and length of the bounding box.
RAYX_FN_ACC
glm::dvec2 RAYX_API cutoutBoundingBox(Cutout cutout) {
    glm::dvec2 ret = glm::dvec2(0.0, 0.0);
    glm::dmat4 keypoints = keyCutoutPoints(cutout);
    for (int i = 0; i < 4; i++) {
        double x = glm::abs(keypoints[i][0]) * 2.0;
        double z = glm::abs(keypoints[i][2]) * 2.0;
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
    glm::dmat4 keypoints = keyCutoutPoints(c1);
    for (int i = 0; i < 4; i++) {
        double x = keypoints[i][0];
        double z = keypoints[i][2];
        _assert(inCutout(c2, x, z), "assertCutoutSubset failed!");
    }
}

}  // namespace RAYX
