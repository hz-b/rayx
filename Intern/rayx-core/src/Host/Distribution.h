#pragma once

namespace rayx::detail::host {

using SeparateValues         = rayx::detail::model::SeparateValues;
using WhiteNoiseDistribution = rayx::detail::model::WhiteNoiseDistribution;
using GaussianDistribution   = rayx::detail::model::GaussianDistribution;

template <typename Dev>
struct BakedDistribution {
    alpaka::Buf<Dev, double, alpaka::DimInt<1>, host::idx> values;
    alpaka::Buf<Dev, double, alpaka::DimInt<1>, host::idx> values;
    bool interpolate;
};

}  // namespace rayx::detail::host

namespace rayx::detail {

inline host::SeparateValues toHost(const model::SeparateValues& dist) { return dist; }

inline host::WhiteNoiseDistribution toHost(const model::WhiteNoiseDistribution& dist) { return dist; }

inline host::GaussianDistribution toHost(const model::GaussianDistribution& dist) { return dist; }

template <typename Dev, typename Q>
inline host::BakedDistribution toHost(Q q, const model::BakedDistribution& dist) {
    auto values  = alpaka::allocAsyncBufIfSupported<double, host::idx>(q, dist.values.size());
    auto weights = alpaka::allocAsyncBufIfSupported<double, host::idx>(q, dist.weights.size());

    alpaka::memcpy(q, values, dist.values.data(), dist.values.size() * sizeof(double));
    alpaka::memcpy(q, weights, dist.weights.data(), dist.weights.size() * sizeof(double));

    return BakedDistribution<Dev>{
        .values      = values,
        .weights     = weights,
        .interpolate = dist.interpolate,
    };
}

}  // namespace rayx::detail
