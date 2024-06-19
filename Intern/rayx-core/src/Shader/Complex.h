#pragma once

#include "Common.h"

#if defined(RAYX_CUDA_ENABLED)
#include <thrust/complex.h>
#else
#include <complex>
#endif

namespace RAYX {

namespace complex {
    template <typename T>
#if defined(RAYX_CUDA_ENABLED)
    using tcomplex = thrust::complex<T>;
#else
    using tcomplex = std::complex<T>;
#endif

    using Complex = tcomplex<double>;
} // namespace complex

using cvec3 = glm::tvec3<complex::Complex>;
using cmat3 = glm::tmat3x3<complex::Complex>;

namespace complex {
#if defined(RAYX_CUDA_ENABLED)
    using thrust::abs;
    using thrust::arg;
    using thrust::norm;
    using thrust::conj;
    using thrust::proj;
    using thrust::polar;
    using thrust::exp;
    using thrust::log;
    using thrust::log10;
    using thrust::pow;
    using thrust::sqrt;

    using thrust::sin;
    using thrust::cos;
    using thrust::tan;
    using thrust::asin;
    using thrust::acos;
    using thrust::atan;

    using thrust::sinh;
    using thrust::cosh;
    using thrust::tanh;
    using thrust::asinh;
    using thrust::acosh;
    using thrust::atanh;
#else
    using std::abs;
    using std::arg;
    using std::norm;
    using std::conj;
    using std::proj;
    using std::polar;
    using std::exp;
    using std::log;
    using std::log10;
    using std::pow;
    using std::sqrt;

    using std::sin;
    using std::cos;
    using std::tan;
    using std::asin;
    using std::acos;
    using std::atan;

    using std::sinh;
    using std::cosh;
    using std::tanh;
    using std::asinh;
    using std::acosh;
    using std::atanh;
#endif
} // namespace complex

} // namespace RAYX

namespace glm {

template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR
mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(
    mat<3, 3, T, Q> const& m1,
    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m2
) {
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

    return mat<3, 3, tcomplex<T>, Q> {
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02,
        SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02,
        SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12,
        SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22,
        SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR
mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(
    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m1,
    mat<3, 3, T, Q> const& m2
) {
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

    return mat<3, 3, tcomplex<T>, Q> {
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02,
        SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02,
        SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12,
        SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22,
        SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR
mat<3, 3, RAYX::complex::tcomplex<T>, Q> operator*(
    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m1,
    mat<3, 3, RAYX::complex::tcomplex<T>, Q> const& m2
) {
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

    return mat<3, 3, tcomplex<T>, Q> {
        SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02,
        SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02,
        SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02,
        SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12,
        SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12,
        SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12,
        SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22,
        SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22,
        SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22,
    };
}

} // namespace glm
