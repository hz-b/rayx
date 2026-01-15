#pragma once

#include <variant>

#include "Math/Constants.h"

namespace rayx::design {

struct WaveLength {
    double value;  // in nanometers
};

struct ElectronVolt {
    double value;  // in eV
};

using PhotonEnergy = std::variant<WaveLength, ElectronVolt>;

inline WaveLength toWaveLength(const WaveLength wavelength) { return wavelength; }

inline WaveLength toWaveLength(const ElectronVolt energy) { return WaveLength{math::constants::inv_nm_to_evolt / energy.value}; }

inline WaveLength toWaveLength(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toWaveLength(arg); }, energy);
}

inline ElectronVolt toElectronVolt(const ElectronVolt energy) { return energy; }

inline ElectronVolt toElectronVolt(const WaveLength wavelength) { return ElectronVolt{math::constants::inv_nm_to_evolt / wavelength.value}; }

inline ElectronVolt toElectronVolt(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toElectronVolt(arg); }, energy);
}

namespace literals {

inline WaveLength operator"" _nm(long double value) { return WaveLength{static_cast<double>(value)}; }

inline ElectronVolt operator"" _eV(long double value) { return ElectronVolt{static_cast<double>(value)}; }

}  // namespace literals

}  // namespace rayx::design
