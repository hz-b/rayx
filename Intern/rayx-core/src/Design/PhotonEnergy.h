#pragma once

#include <variant>

#include "Math/Constants.h"

////////////////////////////////////////////////////////////
// type definitions
////////////////////////////////////////////////////////////

namespace rayx {

struct WaveLength {
    constexpr WaveLength(const double value) { this->value(value); }

    RAYX_VALIDATED_PROPERTY(WaveLength, double, value, detail::validateGreaterZero);  // wavelength in nanometers
};

struct ElectronVolt {
    constexpr ElectronVolt(const double value) { this->value(value); }

    RAYX_VALIDATED_PROPERTY(ElectronVolt, double, value, detail::validateGreaterZero);  // energy in electron volts
};

using PhotonEnergy = std::variant<WaveLength, ElectronVolt>;

}  // namespace rayx

////////////////////////////////////////////////////////////
// conversion functions
////////////////////////////////////////////////////////////

namespace rayx {

constexpr inline WaveLength toWaveLength(const WaveLength wavelength) { return wavelength; }

constexpr inline WaveLength toWaveLength(const ElectronVolt energy) { return WaveLength{constants::inv_nm_to_evolt / energy.value()}; }

constexpr inline WaveLength toWaveLength(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toWaveLength(arg); }, energy);
}

constexpr inline ElectronVolt toElectronVolt(const ElectronVolt energy) { return energy; }

constexpr inline ElectronVolt toElectronVolt(const WaveLength wavelength) { return ElectronVolt{constants::inv_nm_to_evolt / wavelength.value()}; }

constexpr inline ElectronVolt toElectronVolt(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toElectronVolt(arg); }, energy);
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// user-defined literals
////////////////////////////////////////////////////////////

namespace rayx::literals {

constexpr inline WaveLength operator"" _nm(long double value) { return WaveLength{static_cast<double>(value)}; }

constexpr inline ElectronVolt operator"" _eV(long double value) { return ElectronVolt{static_cast<double>(value)}; }

}  // namespace rayx::literals

////////////////////////////////////////////////////////////
// std::ostream operator<< overloads
////////////////////////////////////////////////////////////

#include <ostream>

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::WaveLength wavelength) {
    os << wavelength.value() << "(nm)";
    return os;
}

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::ElectronVolt energy) {
    os << energy.value() << "(eV)";
    return os;
}

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::PhotonEnergy energy) {
    return std::visit([&os](const auto& arg) -> std::ostream& { return os << arg; }, energy);
}

////////////////////////////////////////////////////////////
// std::formatter specializations
////////////////////////////////////////////////////////////

// TODO: implement
