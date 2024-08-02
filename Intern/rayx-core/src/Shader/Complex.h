#pragma once

#include "Common.h"

#if defined(RAYX_CUDA_ENABLED)
#include <cuda/std/complex>
#else
#include <complex>
#endif

namespace RAYX {

namespace complex {
template <typename T>
#if defined(RAYX_CUDA_ENABLED)
using tcomplex = cuda::std::complex<T>;
#else
using tcomplex = std::complex<T>;
#endif

using Complex = tcomplex<double>;
}  // namespace complex

using cvec2 = glm::tvec2<complex::Complex>;
using cvec3 = glm::tvec3<complex::Complex>;
using cmat3 = glm::tmat3x3<complex::Complex>;

namespace complex {
#if defined(RAYX_CUDA_ENABLED)
using cuda::std::abs;
using cuda::std::arg;
using cuda::std::conj;
using cuda::std::exp;
using cuda::std::log;
using cuda::std::log10;
using cuda::std::norm;
using cuda::std::polar;
using cuda::std::pow;
using cuda::std::proj;
using cuda::std::sqrt;

using cuda::std::acos;
using cuda::std::asin;
using cuda::std::atan;
using cuda::std::cos;
using cuda::std::sin;
using cuda::std::tan;

using cuda::std::acosh;
using cuda::std::asinh;
using cuda::std::atanh;
using cuda::std::cosh;
using cuda::std::sinh;
using cuda::std::tanh;
#else
using std::abs;
using std::arg;
using std::conj;
using std::exp;
using std::log;
using std::log10;
using std::norm;
using std::polar;
using std::pow;
using std::proj;
using std::sqrt;

using std::acos;
using std::asin;
using std::atan;
using std::cos;
using std::sin;
using std::tan;

using std::acosh;
using std::asinh;
using std::atanh;
using std::cosh;
using std::sinh;
using std::tanh;
#endif
}  // namespace complex

// add some helper function for glm::tvec of complex
namespace complex {
RAYX_FN_ACC
inline dvec2 abs(cvec2 v) { return dvec2(abs(v.x), abs(v.y)); }

RAYX_FN_ACC
inline dvec3 abs(cvec3 v) { return dvec3(abs(v.x), abs(v.y), abs(v.z)); }

RAYX_FN_ACC
inline dvec2 arg(cvec2 v) { return dvec2(arg(v.x), arg(v.y)); }

RAYX_FN_ACC
inline dvec3 arg(cvec3 v) { return dvec3(arg(v.x), arg(v.y), arg(v.z)); }

RAYX_FN_ACC
inline cvec2 polar(dvec2 mag, dvec2 theta) { return cvec2(polar(mag.x, theta.x), polar(mag.y, theta.y)); }

RAYX_FN_ACC
inline cvec3 polar(dvec3 mag, dvec3 theta) { return cvec3(polar(mag.x, theta.x), polar(mag.y, theta.y), polar(mag.z, theta.z)); }
}  // namespace complex

}  // namespace RAYX

namespace glm {

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(vec<3, RAYX::complex::tcomplex<T>, Q> const& v, T scalar) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(vec<3, T, Q> const& v, RAYX::complex::tcomplex<T> scalar) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(vec<3, RAYX::complex::tcomplex<T>, Q> const& v,
                                                                                 RAYX::complex::tcomplex<T> scalar) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(T scalar, vec<3, RAYX::complex::tcomplex<T>, Q> const& v) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(RAYX::complex::tcomplex<T> scalar, vec<3, T, Q> const& v) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, RAYX::complex::tcomplex<T>, Q> operator*(RAYX::complex::tcomplex<T> scalar,
                                                                                 vec<3, RAYX::complex::tcomplex<T>, Q> const& v) {
    return vec<3, RAYX::complex::tcomplex<T>, Q>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::col_type operator*(
    mat<3, 3, T, Q> const& m, typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::row_type const& v) {
    return typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::col_type(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z, m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z, m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::row_type operator*(
    typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::col_type const& v, mat<3, 3, T, Q> const& m) {
    return typename mat<3, 3, RAYX::complex::tcomplex<T>, Q>::row_type(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z, m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(mat<3, 3, T, Q> const& m1,
                                                                                    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m2) {
    using namespace RAYX::complex;

    T const SrcA00 = m1[0][0];
    T const SrcA01 = m1[0][1];
    T const SrcA02 = m1[0][2];
    T const SrcA10 = m1[1][0];
    T const SrcA11 = m1[1][1];
    T const SrcA12 = m1[1][2];
    T const SrcA20 = m1[2][0];
    T const SrcA21 = m1[2][1];
    T const SrcA22 = m1[2][2];

    tcomplex<T> const SrcB00 = m2[0][0];
    tcomplex<T> const SrcB01 = m2[0][1];
    tcomplex<T> const SrcB02 = m2[0][2];
    tcomplex<T> const SrcB10 = m2[1][0];
    tcomplex<T> const SrcB11 = m2[1][1];
    tcomplex<T> const SrcB12 = m2[1][2];
    tcomplex<T> const SrcB20 = m2[2][0];
    tcomplex<T> const SrcB21 = m2[2][1];
    tcomplex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, tcomplex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m1,
                                                                                    mat<3, 3, T, Q> const& m2) {
    using namespace RAYX::complex;

    tcomplex<T> const SrcA00 = m1[0][0];
    tcomplex<T> const SrcA01 = m1[0][1];
    tcomplex<T> const SrcA02 = m1[0][2];
    tcomplex<T> const SrcA10 = m1[1][0];
    tcomplex<T> const SrcA11 = m1[1][1];
    tcomplex<T> const SrcA12 = m1[1][2];
    tcomplex<T> const SrcA20 = m1[2][0];
    tcomplex<T> const SrcA21 = m1[2][1];
    tcomplex<T> const SrcA22 = m1[2][2];

    T const SrcB00 = m2[0][0];
    T const SrcB01 = m2[0][1];
    T const SrcB02 = m2[0][2];
    T const SrcB10 = m2[1][0];
    T const SrcB11 = m2[1][1];
    T const SrcB12 = m2[1][2];
    T const SrcB20 = m2[2][0];
    T const SrcB21 = m2[2][1];
    T const SrcB22 = m2[2][2];

    return mat<3, 3, tcomplex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template <typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m1,
                                                                                    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m2) {
    using namespace RAYX::complex;

    tcomplex<T> const SrcA00 = m1[0][0];
    tcomplex<T> const SrcA01 = m1[0][1];
    tcomplex<T> const SrcA02 = m1[0][2];
    tcomplex<T> const SrcA10 = m1[1][0];
    tcomplex<T> const SrcA11 = m1[1][1];
    tcomplex<T> const SrcA12 = m1[1][2];
    tcomplex<T> const SrcA20 = m1[2][0];
    tcomplex<T> const SrcA21 = m1[2][1];
    tcomplex<T> const SrcA22 = m1[2][2];

    tcomplex<T> const SrcB00 = m2[0][0];
    tcomplex<T> const SrcB01 = m2[0][1];
    tcomplex<T> const SrcB02 = m2[0][2];
    tcomplex<T> const SrcB10 = m2[1][0];
    tcomplex<T> const SrcB11 = m2[1][1];
    tcomplex<T> const SrcB12 = m2[1][2];
    tcomplex<T> const SrcB20 = m2[2][0];
    tcomplex<T> const SrcB21 = m2[2][1];
    tcomplex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, tcomplex<T>, Q>{
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02, SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02, SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12, SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22, SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

}  // namespace glm
