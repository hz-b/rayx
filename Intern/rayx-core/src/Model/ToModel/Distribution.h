#pragma once

#include "Angle.h"
#include "Design/Distribution.h"
#include "Model/Distribution.h"
#include "PhotonEnergy.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <typename T>
struct ToModel<SeparateValues<T>> {
    static model::SeparateValues apply(const SeparateValues<T>& dist) {
        return model::SeparateValues{
            .center    = toModel(dist.center()),
            .range     = toModel(dist.range()),
            .numValues = toModel(dist.numValues()),
        };
    }
};

template <typename T>
struct ToModel<WhiteNoiseDistribution<T>> {
    static model::WhiteNoiseDistribution apply(const WhiteNoiseDistribution<T>& dist) {
        return model::WhiteNoiseDistribution{
            .center = toModel(dist.center()),
            .range  = toModel(dist.range()),
        };
    }
};

template <typename T>
struct ToModel<GaussianDistribution<T>> {
    static model::GaussianDistribution apply(const GaussianDistribution<T>& dist) {
        return model::GaussianDistribution{
            .mean              = toModel(dist.mean()),
            .standardDeviation = toModel(dist.standardDeviation()),
        };
    }
};

template <typename T>
struct ToModel<BakedDistribution<T>> {
    static model::BakedDistribution apply(const BakedDistribution<T>& dist) {
        return model::BakedDistribution{
            .values      = toModel(dist.values()),
            // if weights are not provided, assume uniform weights
            .weights     = dist.weights() ? toModel(*dist.weights()) : std::vector<double>(dist.values().size(), 1.0),  
            .interpolate = toModel(dist.interpolate()),
        };
    }
};

template <typename T>
struct ToModel<Distribution<T>> {
    static model::Distribution apply(const Distribution<T>& dist) {
        return std::visit([](const auto& d) -> model::Distribution { return toModel(d); }, dist);
    }
};

template <typename T>
struct ToModel<Distribution2D<T>> {
    static model::Distribution2D apply(const Distribution2D<T>& dist) {
        return model::Distribution2D{
            .horizontal = toModel(dist.horizontal()),
            .vertical   = toModel(dist.vertical()),
        };
    }
};

template <typename T>
struct ToModel<Distribution3D<T>> {
    static model::Distribution3D apply(const Distribution3D<T>& dist) {
        return model::Distribution3D{
            .horizontal = toModel(dist.horizontal()),
            .vertical   = toModel(dist.vertical()),
            .depth      = toModel(dist.depth()),
        };
    }
};

}  // namespace rayx::detail
