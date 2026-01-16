#pragma once

namespace rayx::host::detail {

inline math::LocalElectricField toHost(const design::Stokes& stokes) {
    return math::toLocalElectricField(stokes);
}

inline math::LocalElectricField toHost(const design::LocalElectricField& localElectricField) {
    return math::toLocalElectricField(localElectricField);
}

inline math::LocalElectricField toHost(const design::Polarization& polarization) {
    return std::visit( [](const auto& pol) { return toHost(pol); }, polarization);
}

}  // namespace rayx::host::detail
