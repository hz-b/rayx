#pragma once

#include "Design/Polarization.h"
#include "Math/ElectricField.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<Stokes> {
    static LocalElectricField apply(const Stokes& stokes) { return toLocalElectricField(stokes); }
};

template <>
struct ToModel<LocalElectricField> {
    static LocalElectricField apply(const LocalElectricField& localElectricField) { return localElectricField; }
};

template <>
struct ToModel<Polarization> {
    static LocalElectricField apply(const Polarization& polarization) {
        return std::visit([](const auto& pol) -> LocalElectricField { return toModel(pol); }, polarization);
    }
};

}  // namespace rayx::detail
