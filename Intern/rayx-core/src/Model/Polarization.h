#pragma once

#include "Design/Polarization.h"

namespace rayx::detail {

inline LocalElectricField toModel(const Stokes& stokes) { return toLocalElectricField(stokes); }
inline LocalElectricField toModel(const LocalElectricField& localElectricField) { return toLocalElectricField(localElectricField); }
inline LocalElectricField toModel(const Polarization& polarization) {
    return std::visit([](const auto& pol) -> LocalElectricField { return toModel(pol); }, polarization);
}

}  // namespace rayx::detail
