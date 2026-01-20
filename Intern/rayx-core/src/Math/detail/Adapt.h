#pragma once

#include <complex>
#include <variant>

namespace rayx::math::detail {

/// adapt struct to adapt complex numbers and variants to a common interface
template <template <typename> typename T>
struct adapt;

/// specialize adapt for std::complex
template <>
struct adapt<std::complex> {
// clang-format off
    static auto real(const std::complex<double> c) { return std::real(c); }
    static auto imag(const std::complex<double> c) { return std::imag(c); }
    static auto abs(const std::complex<double> c) { return std::abs(c); }
    static auto arg(const std::complex<double> c) { return std::arg(c); }
    static auto norm(const std::complex<double> c) { return std::norm(c); }
    static auto conj(const std::complex<double> c) { return std::conj(c); }
    static auto proj(const std::complex<double> c) { return std::proj(c); }
    static auto polar(double magnitude, double angle) { return std::polar(magnitude, angle); }
    static auto exp(const std::complex<double> c) { return std::exp(c); }
    static auto log(const std::complex<double> c) { return std::log(c); }
    static auto log10(const std::complex<double> c) { return std::log10(c); }
    static auto pow(const std::complex<double> base, const int exp) { return std::pow(base, exp); }
    static auto pow(const std::complex<double> base, const std::complex<double> exp) { return std::pow(base, exp); }
    static auto pow(const std::complex<double> base, const double exp) { return std::pow(base, exp); }
    static auto pow(const double base, const std::complex<double> exp) { return std::pow(base, exp); }
    static auto sqrt(const std::complex<double> c) { return std::sqrt(c); }
    static auto sin(const std::complex<double> c) { return std::sin(c); }
    static auto cos(const std::complex<double> c) { return std::sin(c); }
    static auto tan(const std::complex<double> c) { return std::tan(c); }
    static auto asin(const std::complex<double> c) { return std::asin(c); }
    static auto acos(const std::complex<double> c) { return std::acos(c); }
    static auto atan(const std::complex<double> c) { return std::atan(c); }
    static auto sinh(const std::complex<double> c) { return std::sinh(c); }
    static auto cosh(const std::complex<double> c) { return std::cosh(c); }
    static auto tanh(const std::complex<double> c) { return std::tanh(c); }
    static auto asinh(const std::complex<double> c) { return std::asinh(c); }
    static auto acosh(const std::complex<double> c) { return std::acosh(c); }
    static auto atanh(const std::complex<double> c) { return std::atanh(c); }
// clang-format on
};

/// specialize adapt for std::variant
template <>
struct adapt<std::variant> {
// clang-format off
    template <typename Visitor, typename ...Variants> static auto visit(Visitor&& vis, Variants&&... var) { return std::visit(std::forward<Visitor>(vis), std::forward<Variants>(var)...); }
    template <typename R, typename Visitor, typename ...Variants> static auto visit(Visitor&& vis, Variants&&... var) { return std::visit<R>(std::forward<Visitor>(vis), std::forward<Variants>(var)...); }
// clang-format on
};

/// type trait `LocalElectricField_T` to define type `LocalElectricField` based on complex number type
template <template <typename> typename TComplex>
struct LocalElectricField_T;

/// helper alias template for type trait LocalElectricField_T
template <template <typename> typename TComplex>
using LocalElectricField_t = typename LocalElectricField_T<TComplex>::type;

/// type trait `ElectricField_T` to define type `ElectricField` based on complex number type
template <template <typename> typename TComplex>
struct ElectricField_T;

/// helper alias template for type trait ElectricField_T
template <template <typename> typename TComplex>
using ElectricField_t = typename ElectricField_T<TComplex>::type;

} // namespace
