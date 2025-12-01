#pragma once

#include <variant>

#include "Constants.h"

namespace rayx {

struct WaveLength {
    double value;  // in nanometers
};

struct ElectronVolt {
    double value;  // in eV
};

WaveLength toWaveLength(const WaveLength wavelength) { return wavelength; }

WaveLength toWaveLength(const ElectronVolt energy) { return WaveLength{INV_NM_TO_EVOLT / energy.value}; }

WaveLength toWaveLength(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toWaveLength(arg); }, energy);
}

ElectronVolt toElectronVolt(const ElectronVolt energy) { return energy; }

ElectronVolt toElectronVolt(const WaveLength wavelength) { return ElectronVolt{INV_NM_TO_EVOLT / wavelength.value}; }

ElectronVolt toElectronVolt(const PhotonEnergy energy) {
    return std::visit([](auto&& arg) { return toElectronVolt(arg); }, energy);
}

using PhotonEnergy       = std::variant<WaveLength, ElectronVolt>;

namespace literals {

WaveLength operator"" _nm(long double value) {
    return WaveLength{static_cast<double>(value)};
}

ElectronVolt operator"" _eV(long double value) {
    return ElectronVolt{static_cast<double>(value)};
}

} // namespace literals

} // namespace rayx
