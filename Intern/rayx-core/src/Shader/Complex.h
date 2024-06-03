#pragma once

#include "Common.h"
#include <alpaka/math/Complex.hpp>

using complex = alpaka::Complex<double>;
using dvec3c = glm::tvec3<complex>;
using dmat3c = glm::tmat3x3<complex>;

namespace glm {

template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER GLM_CONSTEXPR
mat<3, 3, alpaka::Complex<T>, Q> operator*(
    mat<3, 3, T, Q> const& m1,
    mat<3, 3, alpaka::Complex<T>, Q> const& m2
) {
    T const SrcA00 = m1[0][0];
    T const SrcA01 = m1[0][1];
    T const SrcA02 = m1[0][2];
    T const SrcA10 = m1[1][0];
    T const SrcA11 = m1[1][1];
    T const SrcA12 = m1[1][2];
    T const SrcA20 = m1[2][0];
    T const SrcA21 = m1[2][1];
    T const SrcA22 = m1[2][2];

    alpaka::Complex<T> const SrcB00 = m2[0][0];
    alpaka::Complex<T> const SrcB01 = m2[0][1];
    alpaka::Complex<T> const SrcB02 = m2[0][2];
    alpaka::Complex<T> const SrcB10 = m2[1][0];
    alpaka::Complex<T> const SrcB11 = m2[1][1];
    alpaka::Complex<T> const SrcB12 = m2[1][2];
    alpaka::Complex<T> const SrcB20 = m2[2][0];
    alpaka::Complex<T> const SrcB21 = m2[2][1];
    alpaka::Complex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, alpaka::Complex<T>, Q> {
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
mat<3, 3, alpaka::Complex<T>, Q> operator*(
    mat<3, 3, alpaka::Complex<T>, Q> const& m1,
    mat<3, 3, T, Q> const& m2
) {
    alpaka::Complex<T> const SrcA00 = m1[0][0];
    alpaka::Complex<T> const SrcA01 = m1[0][1];
    alpaka::Complex<T> const SrcA02 = m1[0][2];
    alpaka::Complex<T> const SrcA10 = m1[1][0];
    alpaka::Complex<T> const SrcA11 = m1[1][1];
    alpaka::Complex<T> const SrcA12 = m1[1][2];
    alpaka::Complex<T> const SrcA20 = m1[2][0];
    alpaka::Complex<T> const SrcA21 = m1[2][1];
    alpaka::Complex<T> const SrcA22 = m1[2][2];

    T const SrcB00 = m2[0][0];
    T const SrcB01 = m2[0][1];
    T const SrcB02 = m2[0][2];
    T const SrcB10 = m2[1][0];
    T const SrcB11 = m2[1][1];
    T const SrcB12 = m2[1][2];
    T const SrcB20 = m2[2][0];
    T const SrcB21 = m2[2][1];
    T const SrcB22 = m2[2][2];

    return mat<3, 3, alpaka::Complex<T>, Q> {
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
mat<3, 3, alpaka::Complex<T>, Q> operator*(
    mat<3, 3, alpaka::Complex<T>, Q> const& m1,
    mat<3, 3, alpaka::Complex<T>, Q> const& m2
) {
    alpaka::Complex<T> const SrcA00 = m1[0][0];
    alpaka::Complex<T> const SrcA01 = m1[0][1];
    alpaka::Complex<T> const SrcA02 = m1[0][2];
    alpaka::Complex<T> const SrcA10 = m1[1][0];
    alpaka::Complex<T> const SrcA11 = m1[1][1];
    alpaka::Complex<T> const SrcA12 = m1[1][2];
    alpaka::Complex<T> const SrcA20 = m1[2][0];
    alpaka::Complex<T> const SrcA21 = m1[2][1];
    alpaka::Complex<T> const SrcA22 = m1[2][2];

    alpaka::Complex<T> const SrcB00 = m2[0][0];
    alpaka::Complex<T> const SrcB01 = m2[0][1];
    alpaka::Complex<T> const SrcB02 = m2[0][2];
    alpaka::Complex<T> const SrcB10 = m2[1][0];
    alpaka::Complex<T> const SrcB11 = m2[1][1];
    alpaka::Complex<T> const SrcB12 = m2[1][2];
    alpaka::Complex<T> const SrcB20 = m2[2][0];
    alpaka::Complex<T> const SrcB21 = m2[2][1];
    alpaka::Complex<T> const SrcB22 = m2[2][2];

    return mat<3, 3, alpaka::Complex<T>, Q> {
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
