#pragma once

#include "Design/PhotonEnergy.h"

namespace rayx::detail {

inline double toModel(const ElectronVolt& electronVolt) { return toElectronVolt(electronVolt).value(); }

inline double toModel(const WaveLength& waveLength) { return toElectronVolt(waveLength).value(); }

inline double toModel(const PhotonEnergy& photonEnergy) {
    return std::visit([](const auto& energy) { return toModel(energy); }, photonEnergy);
}

}  // namespace rayx::detail
