#pragma once

#include "Design/Polarization.h"
#include "ToModel.h"

namespace rayx::detail {

inline LocalElectricField toModel(const Stokes& stokes) { return toLocalElectricField(stokes); }
inline LocalElectricField toModel(const LocalElectricField& localElectricField) { return toLocalElectricField(localElectricField); }

}
