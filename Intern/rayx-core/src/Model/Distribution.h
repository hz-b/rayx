#pragma once

#include "Design/Distribution.h"
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

}  // namespace rayx::detail::model

namespace rayx::detail {

template <typename T>
model::SeparateValues toModel(const design::SeparateValues<T>& dst) {
    return model::SeparateValues{
        .center    = toModel(dist.center()),
        .range     = toModel(dist.range()),
        .numValues = toModel(dist.numValues()),
    };
}

template <typename T>
model::WhiteNoiseDistribution toModel(const design::WhiteNoiseDistribution<T>& dist) {
    return model::WhiteNoiseDistribution{
        .center = toModel(dist.center()),
        .range  = toModel(dist.range()),
    };
}

template <typename T>
model::GaussianDistribution toModel(const design::GaussianDistribution<T>& dist) {
    return model::GaussianDistribution{
        .mean              = toModel(dist.mean()),
        .standardDeviation = toModel(dist.standardDeviation()),
    };
}

template <typename T>
model::BakedDistribution toModel(const design::BakedDistribution<T>& dist) {
    return model::BakedDistribution{
        .values      = toModel(dist.values()),
        .weights     = dist.weights() ? *dist.weights() : std::vector<double>(dist.values().size(), 1.0),
        .interpolate = toModel(dist.interpolate()),
    };
}

}  // namespace rayx::detail
