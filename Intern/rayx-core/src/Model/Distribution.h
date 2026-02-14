#pragma once

#include "Angle.h"
#include "Design/Distribution.h"
#include "PhotonEnergy.h"

namespace rayx::detail {

namespace host {

struct SeparateValues {
    double center;
    double range;
    int numValues;
};

struct WhiteNoiseDistribution {
    double center;
    double range;
};

struct GaussianDistribution {
    double mean;
    double standardDeviation;
};

struct BakedDistribution {
    alpaka::Buf<double, alpaka::Dim<1>, alpaka::Idx<1>> values;
    std::optional<alpaka::Buf<double, alpaka::Dim<1>, alpaka::Idx<1>>> weights;
    bool interpolate;
};

} // namespace rayx::detail::host

template <typename T>
host::SeparateValues toHost(const design::SeparateValues<T>& separateValues) {
    return host::SeparateValues{
        .center    = toHost(separateValues.center),
        .range     = toHost(separateValues.range),
        .numValues = separateValues.numValues,
    };
}

template <typename T>
host::WhiteNoiseDistribution toHost(const design::WhiteNoiseDistribution<T>& dist) {
    return host::WhiteNoiseDistribution{
        .center = toHost(dist.center),
        .range  = toHost(dist.range),
    };
}

template <typename T>
requires std::is_arithmetic_v<T>
host::GaussianDistribution toHost(const design::GaussianDistribution<T>& dist) {
    return host::GaussianDistribution{
        .mean              = toHost(dist.mean),
        .standardDeviation = toHost(dist.standardDeviation),
    };
}

template <ArithmeticStrongTypeDef T>
host::GaussianDistribution toHost(const design::GaussianDistribution<T>& dist) {
    auto standardDeviation = mean;
    standardDeviation.value(dist.standardDeviation);

    return host::GaussianDistribution{
        .mean              = toHost(dist.mean),
        .standardDeviation = toHost(dist.standardDeviation),
    };
}

template <ArithmeticStrongTypeDef ...Ts>
host::GaussianDistribution toHost<std::variant<Ts...>(const design::GaussianDistribution<T>& dist) {
    const auto standardDeviation = std::visit([&]<typename T>(const T&) { return toHost(T(dist.standardDeviation)) }, dist.mean);

    return host::GaussianDistribution{
        .mean              = toHost(dist.mean),
        .standardDeviation = toHost(dist.standardDeviation),
    };
}

host::BakedDistribution toHost(const std::shared_ptr<design::BakedDistribution<double>>& dist) {
    host::BakedDistribution hostDist;
    hostDist.values = alpaka::allocBuf<double, alpaka::Dim<1>, alpaka::Idx<1>>(rayx::getDefaultHostAllocator(), dist->values().size());
    for (size_t i = 0; i < dist->values().size(); ++i) { (*hostDist.values)[i] = toHost(dist->values()[i]); }
    if (dist->weights()) {
        hostDist.weights = alpaka::allocBuf<double, alpaka::Dim<1>, alpaka::Idx<1>>(rayx::getDefaultHostAllocator(), dist->weights()->size());
        for (size_t i = 0; i < dist->weights()->size(); ++i) { (*hostDist.weights)->operator[](i) = toHost((*dist->weights())[i]); }
    }
    hostDist.interpolate = dist->interpolate;
    return hostDist;
}

namespace device {

using SeparateValues         = rayx::detail::host::SeparateValues;
using WhiteNoiseDistribution = rayx::detail::host::WhiteNoiseDistribution;
using GaussianDistribution   = rayx::detail::host::GaussianDistribution;

struct BakedDistribution {
    double* __restrict values;
    double* __restrict weights; // optional, can be nullptr
    bool interpolate;
};

} // namespace rayx::detail::device

device::SeparateValues toDevice(const host::SeparateValues& separateValues);
device::WhiteNoiseDistribution toDevice(const host::WhiteNoiseDistribution& dist);
device::GaussianDistribution toDevice(const host::GaussianDistribution& dist);
device::BakedDistribution toDevice(const host::BakedDistribution& dist);

using SeparateValues         = rayx::design::SeparateValues<double>;
using WhiteNoiseDistribution = rayx::design::WhiteNoiseDistribution<double>;
using GaussianDistribution   = rayx::design::GaussianDistribution<double>;
using BakedDistribution      = rayx::design::BakedDistribution<double>;
using Distribution           = rayx::design::Distribution<double>;
using Distribution2D         = rayx::design::Distribution2D<double>;
using Distribution3D         = rayx::design::Distribution3D<double>;

template <typename T>
SeparateValues<double> toHost(const design::SeparateValues<T>& separateValues) {
    return SeparateValues<double>{
        .center    = toHost(separateValues.center),
        .range     = toHost(separateValues.range),
        .numValues = separateValues.numValues,
    };
}

template <typename T>
WhiteNoiseDistribution<double> toHost(const design::WhiteNoiseDistribution<T>& dist) {
    return WhiteNoiseDistribution<double>{
        .center = toHost(dist.center),
        .range  = toHost(dist.range),
    };
}

template <typename T>
GaussianDistribution<double> toHost(const design::GaussianDistribution<T>& dist) {
    return GaussianDistribution<double>{
        .mean              = toHost(dist.mean),
        .standardDeviation = toHost(dist.standardDeviation),
    };
}

template <typename T>
BakedDistribution<double> toHost(const std::shared_ptr<design::BakedDistribution<T>>& dist) {
    BakedDistribution<double> hostDist;
    hostDist.values.resize(dist->values->size());
    for (size_t i = 0; i < dist->values->size(); ++i) { hostDist.values[i] = toHost((*dist->values)[i]); }
    if (dist->weights.has_value()) {
        hostDist.weights = std::vector<double>(dist->weights->size());
        for (size_t i = 0; i < dist->weights->size(); ++i) { (*hostDist.weights)[i] = (*dist->weights)[i]; }
    }
    hostDist.interpolate = dist->interpolate;
    return hostDist;
}

template <typename T>
Distribution<double> toHost(const design::Distribution<T>& distribution) {
    return std::visit([&](const auto& dist) { return toHost(dist); }, distribution);
}

template <typename T>
Distribution2D<double> toHost(const design::Distribution2D<T>& dist2D) {
    return Distribution2D<double>{
        .x = toHost(dist2D.x),
        .y = toHost(dist2D.y),
    };
}

template <typename T>
Distribution3D<double> toHost(const design::Distribution3D<T>& dist3D) {
    return Distribution3D<double>{
        .x = toHost(dist3D.x),
        .y = toHost(dist3D.y),
        .z = toHost(dist3D.z),
    };
}

}  // namespace rayx::host::detail
