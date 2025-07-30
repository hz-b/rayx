#pragma once

#include <algorithm>
#include <complex>
#include <glm.hpp>

#include "Core.h"

namespace RAYX {

using cvec2 = glm::tvec2<std::complex<double>>;
using cvec3 = glm::tvec3<std::complex<double>>;
using cmat3 = glm::tmat3x3<std::complex<double>>;

}  // namespace RAYX

namespace std {

// add some helper function for glm::tvec of complex
RAYX_FN_ACC
inline glm::dvec2 abs(RAYX::cvec2 v) { return glm::dvec2(abs(v.x), abs(v.y)); }

RAYX_FN_ACC
inline glm::dvec3 abs(RAYX::cvec3 v) { return glm::dvec3(abs(v.x), abs(v.y), abs(v.z)); }

RAYX_FN_ACC
inline glm::dvec2 arg(RAYX::cvec2 v) { return glm::dvec2(arg(v.x), arg(v.y)); }

RAYX_FN_ACC
inline glm::dvec3 arg(RAYX::cvec3 v) { return glm::dvec3(arg(v.x), arg(v.y), arg(v.z)); }

RAYX_FN_ACC
inline RAYX::cvec2 polar(glm::dvec2 mag, glm::dvec2 theta) { return RAYX::cvec2(polar(mag.x, theta.x), polar(mag.y, theta.y)); }

RAYX_FN_ACC
inline RAYX::cvec3 polar(glm::dvec3 mag, glm::dvec3 theta) { return RAYX::cvec3(polar(mag.x, theta.x), polar(mag.y, theta.y), polar(mag.z, theta.z)); }

}

namespace glm {

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(vec<3, std::complex<T>, Q> const& v, T scalar) {
    return vec<3, std::complex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(vec<3, T, Q> const& v, std::complex<T> scalar) {
    return vec<3, std::complex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(vec<3, std::complex<T>, Q> const& v,
                                                                                 std::complex<T> scalar) {
    return vec<3, std::complex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(T scalar, vec<3, std::complex<T>, Q> const& v) {
    return vec<3, std::complex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(std::complex<T> scalar, vec<3, T, Q> const& v) {
    return vec<3, std::complex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, std::complex<T>, Q> operator*(std::complex<T> scalar,
                                                                                 vec<3, std::complex<T>, Q> const& v) {
    return vec<3, std::complex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR typename mat<3, 3, std::complex<T>, Q>::col_type operator*(
    mat<3, 3, T, Q> const& m, typename mat<3, 3, std::complex<T>, Q>::row_type const& v) {
    return typename mat<3, 3, std::complex<T>, Q>::col_type(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z, m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z, m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR typename mat<3, 3, std::complex<T>, Q>::row_type operator*(
    typename mat<3, 3, std::complex<T>, Q>::col_type const& v, mat<3, 3, T, Q> const& m) {
    return typename mat<3, 3, std::complex<T>, Q>::row_type(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z, m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, std::complex<T>, Q> operator*(mat<3, 3, T, Q> const& m1,
                                                                                    mat<3, 3, std::complex<T>, Q> const& m2) {
    T const SrcA00 = m1[0][0];
    T const SrcA01 = m1[0][1];
    T const SrcA02 = m1[0][2];
    T const SrcA10 = m1[1][0];
    T const SrcA11 = m1[1][1];
    T const SrcA12 = m1[1][2];
    T const SrcA20 = m1[2][0];
    T const SrcA21 = m1[2][1];
    T const SrcA22 = m1[2][2];

    std::complex<T> const SrcB00 = m2[0][0];
    std::complex<T> const SrcB01 = m2[0][1];
    std::complex<T> const SrcB02 = m2[0][2];
    std::complex<T> const SrcB10 = m2[1][0];
    std::complex<T> const SrcB11 = m2[1][1];
    std::complex<T> const SrcB12 = m2[1][2];
    std::complex<T> const SrcB20 = m2[2][0];
    std::complex<T> const SrcB21 = m2[2][1];
    std::complex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, std::complex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, std::complex<T>, Q> operator*(mat<3, 3, std::complex<T>, Q> const& m1,
                                                                                    mat<3, 3, T, Q> const& m2) {
    std::complex<T> const SrcA00 = m1[0][0];
    std::complex<T> const SrcA01 = m1[0][1];
    std::complex<T> const SrcA02 = m1[0][2];
    std::complex<T> const SrcA10 = m1[1][0];
    std::complex<T> const SrcA11 = m1[1][1];
    std::complex<T> const SrcA12 = m1[1][2];
    std::complex<T> const SrcA20 = m1[2][0];
    std::complex<T> const SrcA21 = m1[2][1];
    std::complex<T> const SrcA22 = m1[2][2];

    T const SrcB00 = m2[0][0];
    T const SrcB01 = m2[0][1];
    T const SrcB02 = m2[0][2];
    T const SrcB10 = m2[1][0];
    T const SrcB11 = m2[1][1];
    T const SrcB12 = m2[1][2];
    T const SrcB20 = m2[2][0];
    T const SrcB21 = m2[2][1];
    T const SrcB22 = m2[2][2];

    return mat<3, 3, std::complex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, std::complex<T>, Q> operator*(mat<3, 3, std::complex<T>, Q> const& m1,
                                                                                    mat<3, 3, std::complex<T>, Q> const& m2) {
    std::complex<T> const SrcA00 = m1[0][0];
    std::complex<T> const SrcA01 = m1[0][1];
    std::complex<T> const SrcA02 = m1[0][2];
    std::complex<T> const SrcA10 = m1[1][0];
    std::complex<T> const SrcA11 = m1[1][1];
    std::complex<T> const SrcA12 = m1[1][2];
    std::complex<T> const SrcA20 = m1[2][0];
    std::complex<T> const SrcA21 = m1[2][1];
    std::complex<T> const SrcA22 = m1[2][2];

    std::complex<T> const SrcB00 = m2[0][0];
    std::complex<T> const SrcB01 = m2[0][1];
    std::complex<T> const SrcB02 = m2[0][2];
    std::complex<T> const SrcB10 = m2[1][0];
    std::complex<T> const SrcB11 = m2[1][1];
    std::complex<T> const SrcB12 = m2[1][2];
    std::complex<T> const SrcB20 = m2[2][0];
    std::complex<T> const SrcB21 = m2[2][1];
    std::complex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, std::complex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

}  // namespace glm
