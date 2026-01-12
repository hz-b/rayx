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

WaveLength toWaveLength(const WaveLength wavelength) { return wavelength; }

WaveLength toWaveLength(const ElectronVolt energy) { return WaveLength{math::constants::inv_nm_to_evolt / energy.value}; }

WaveLength toWaveLength(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toWaveLength(arg); }, energy);
}

ElectronVolt toElectronVolt(const ElectronVolt energy) { return energy; }

ElectronVolt toElectronVolt(const WaveLength wavelength) { return ElectronVolt{math::constants::inv_nm_to_evolt / wavelength.value}; }

ElectronVolt toElectronVolt(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toElectronVolt(arg); }, energy);
}

namespace literals {

WaveLength operator"" _nm(long double value) { return WaveLength{static_cast<double>(value)}; }

ElectronVolt operator"" _eV(long double value) { return ElectronVolt{static_cast<double>(value)}; }

}  // namespace literals

}  // namespace rayx::design
