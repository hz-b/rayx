#pragma once

#include <variant>

#include "Math/ElectricField.h"

namespace rayx {
constexpr inline double degreeOfPolarization(double I, double Q, double U, double V);
}  // namespace rayx

////////////////////////////////////////////////////////////
// type definitions
////////////////////////////////////////////////////////////

namespace rayx {

// TODO: support partially polarized light. (i.e. Stokes represent a distribution of polarization states)
// For now only fully polarized light is supported, thus Stokes enforce this constraint in the constructor.
// In the future, if we want to support partially polarized light, we can relax this constraint.
// In that case, we can remove the validation of degree of polarization.
struct Stokes {
    constexpr Stokes(double I, double Q, double U, double V) : m_I(I), m_Q(Q), m_U(U), m_V(V) {
        detail::validateGreaterEqualZero("Stokes", "I", I);
        detail::validateInRange("Stokes", "Q", Q, -I, I, "Q must be between -I and I");
        detail::validateInRange("Stokes", "U", U, -I, I, "U must be between -I and I");
        detail::validateInRange("Stokes", "V", V, -I, I, "V must be between -I and I");
        detail::validateEqual("Stokes", "DegreeOfPolarization", degreeOfPolarization(I, Q, U, V), 1.0,
                              "degree of polarization must be equal to 1 for fully polarized light. fully polarized "
                              "Stokes parameters must satisfy "
                              "the relation I^2 = Q^2 + U^2 + V^2");
    }

    double I() const { return m_I; }
    double Q() const { return m_Q; }
    double U() const { return m_U; }
    double V() const { return m_V; }

  private:
    double m_I;  ///< Total intensity (I)
    double m_Q;  ///< Linear polarization component (Q)
    double m_U;  ///< Linear polarization component (U)
    double m_V;  ///< Circular polarization component (V)
};

using Polarization = std::variant<Stokes, LocalElectricField>;

}  // namespace rayx

////////////////////////////////////////////////////////////
// conversion functions
////////////////////////////////////////////////////////////

namespace rayx {

inline Stokes toStokes(const LocalElectricField field) {
    const auto mag   = glm::dvec2(std::abs(field.x), std::abs(field.y));
    const auto theta = glm::dvec2(std::arg(field.x), std::arg(field.y));

    return Stokes(mag.x * mag.x + mag.y * mag.y, mag.x * mag.x - mag.y * mag.y, 2.0 * mag.x * mag.y * std::cos(theta.x - theta.y),
                  2.0 * mag.x * mag.y * std::sin(theta.x - theta.y));
}

inline Stokes toStokes(const Stokes& stokes) { return stokes; }

inline Stokes toStokes(const Polarization& polarization) {
    return std::visit([](const auto& pol) { return toStokes(pol); }, polarization);
}

inline LocalElectricField toLocalElectricField(const LocalElectricField& field) { return field; }

inline LocalElectricField toLocalElectricField(const Stokes stokes) {
    const auto x_real  = std::sqrt((stokes.I() + stokes.Q()) / 2.0);
    const auto y_mag   = std::sqrt((stokes.I() - stokes.Q()) / 2.0);
    const auto y_theta = -1.0 * std::atan2(stokes.V(), stokes.U());
    const auto y       = std::polar(y_mag, y_theta);
    return LocalElectricField({x_real, 0}, y);
}

inline LocalElectricField toLocalElectricField(const Polarization& polarization) {
    return std::visit([](const auto& pol) { return toLocalElectricField(pol); }, polarization);
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// utility functions
////////////////////////////////////////////////////////////

namespace rayx {

constexpr inline double degreeOfPolarization(double I, double Q, double U, double V) { return std::sqrt(Q * Q + U * U + V * V) / I; }

constexpr inline double intensity(const Stokes& stokes) { return stokes.I(); }

constexpr inline double intensity(const Polarization& polarization) {
    return std::visit([](const auto& pol) { return intensity(pol); }, polarization);
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// std::ostream operator<< overloads
////////////////////////////////////////////////////////////

#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const rayx::Stokes stokes) {
    return os << "{I=" << stokes.I() << ", Q=" << stokes.Q() << ", U=" << stokes.U() << ", V=" << stokes.V() << "}";
}

inline std::ostream& operator<<(std::ostream& os, const rayx::Polarization polarization) {
    return std::visit([&os](const auto& pol) -> std::ostream& { return os << pol; }, polarization);
}

////////////////////////////////////////////////////////////
// std::formatter specializations
////////////////////////////////////////////////////////////

// TODO: implement
