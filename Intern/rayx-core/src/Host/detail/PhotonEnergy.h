#pragma once

#include "Design/PhotonEnergy.h"

namespace rayx::host::detail {

double toHost(const design::ElectronVolt energy) {
    return design::toElectronVolt(energy).value;
}

double toHost(const design::WaveLength waveLength) {
    return design::toElectronVolt(waveLength).value;
}

double toHost(const design::PhotonEnergy photonEnergy) {
    return design::toElectronVolt(photonEnergy).value;
}

}
