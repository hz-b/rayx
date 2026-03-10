#pragma once

#include "Design/PhotonEnergy.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<ElectronVolt> {
    static double apply(const ElectronVolt& electronVolt) { return toElectronVolt(electronVolt).value(); }
};

template <>
struct ToModel<WaveLength> {
    static double apply(const WaveLength& waveLength) { return toElectronVolt(waveLength).value(); }
};

template <>
struct ToModel<PhotonEnergy> {
    static double apply(const PhotonEnergy& photonEnergy) {
        return std::visit([](const auto& energy) { return toModel(energy); }, photonEnergy);
    }
};

}  // namespace rayx::detail
