#pragma once

namespace rayx::detail::device {

using SeparateValues         = rayx::detail::host::SeparateValues;
using WhiteNoiseDistribution = rayx::detail::host::WhiteNoiseDistribution;
using GaussianDistribution   = rayx::detail::host::GaussianDistribution;

struct BakedDistribution {
    double* __restrict values;
    double* __restrict weights;
    bool interpolate;
};

}  // namespace rayx::detail::device

namespace rayx::detail {

inline device::SeparateValues toDevice(const host::SeparateValues& dist) { return dist; }

inline device::WhiteNoiseDistribution toDevice(const host::WhiteNoiseDistribution& dist) { return dist; }

inline device::GaussianDistribution toDevice(const host::GaussianDistribution& dist) { return dist; }

template <typename Dev>
inline device::BakedDistribution toDevice(const host::BakedDistribution<Dev>& dist) {
    return device::BakedDistribution{
        .values      = alpaka::getPtrNative(dist.values),
        .weights     = alpaka::getPtrNative(dist.weights),
        .interpolate = dist.interpolate,
    };
}

}  // namespace rayx::detail
