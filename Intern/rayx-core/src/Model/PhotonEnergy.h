#pragma once

#include "Design/PhotonEnergy.h"
#include "ToModel.h"

namespace rayx::detail {

inline double toModel(const ElectronVolt electronVolt) { return toElectronVolt(electronVolt).value(); }
inline double toModel(const WaveLength waveLength) { return toElectronVolt(waveLength).value(); }

}  // namespace rayx::detail
