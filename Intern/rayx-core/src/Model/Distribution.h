#pragma once

#include "Angle.h"
#include "Design/Distribution.h"
#include "PhotonEnergy.h"
#include "Polarization.h"
#include "ToModel.h"

namespace rayx::detail::model {

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
    std::vector<double> values;
    std::vector<double> weights;
    bool interpolate;
};

using Distribution = std::variant<double, SeparateValues, WhiteNoiseDistribution, GaussianDistribution, BakedDistribution>;

struct Distribution2D {
    Distribution horizontal;
    Distribution vertical;
};

struct Distribution3D {
    Distribution horizontal;
    Distribution vertical;
    Distribution depth;
};

}  // namespace rayx::detail::model

namespace rayx::detail {

template <typename T>
inline model::SeparateValues toModel(const SeparateValues<T>& dist) {
    return model::SeparateValues{
        .center    = toModel(dist.center()),
        .range     = toModel(dist.range()),
        .numValues = toModel(dist.numValues()),
    };
}

template <typename T>
inline model::WhiteNoiseDistribution toModel(const WhiteNoiseDistribution<T>& dist) {
    return model::WhiteNoiseDistribution{
        .center = toModel(dist.center()),
        .range  = toModel(dist.range()),
    };
}

template <typename T>
inline model::GaussianDistribution toModel(const GaussianDistribution<T>& dist) {
    return model::GaussianDistribution{
        .mean              = toModel(dist.mean()),
        .standardDeviation = toModel(dist.standardDeviation()),
    };
}

template <typename T>
inline model::BakedDistribution toModel(const BakedDistribution<T>& dist) {
    return model::BakedDistribution{
        .values      = toModel(dist.values()),
        .weights     = dist.weights() ? *dist.weights() : std::vector<double>(dist.values().size(), 1.0),
        .interpolate = dist.interpolate(),
    };
}

template <typename T>
inline model::Distribution toModel(const Distribution<T>& dist) {
    return std::visit([](const auto& d) -> model::Distribution { return toModel(d); }, dist);
}

template <typename T>
inline model::Distribution2D toModel(const Distribution2D<T>& dist) {
    return model::Distribution2D{
        .horizontal = toModel(dist.horizontal()),
        .vertical   = toModel(dist.vertical()),
    };
}

template <typename T>
inline model::Distribution3D toModel(const Distribution3D<T>& dist) {
    return model::Distribution3D{
        .horizontal = toModel(dist.horizontal()),
        .vertical   = toModel(dist.vertical()),
        .depth      = toModel(dist.depth()),
    };
}

}  // namespace rayx::detail
