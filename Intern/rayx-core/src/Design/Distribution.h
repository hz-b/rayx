#pragma once

namespace rayx::design {

template <typename T>
struct SeparateValues {
    T center      = T();
    T range       = T();
    int numValues = 1;
};

template <typename T>
struct WhiteNoiseDistribution {
    T center = T();
    T range  = T();
};

template <typename T>
struct GaussianDistribution {
    T mean              = T();
    T standardDeviation = T();
};

template <typename T>
struct BakedDistribution {
    std::vector<T> values = std::make_shared<std::vector<T>>();
    std::optional<std::vector<T>> weights;
    bool interpolate = false;
};

template <typename T>
using Distribution = std::variant<T, SeparateValues<T>, WhiteNoiseDistribution<T>, GaussianDistribution<T>, std::shared_ptr<BakedDistribution<T>>>;

template <typename T>
struct Distribution2D {
    Distribution<T> x;
    Distribution<T> y;
};

template <typename T>
struct Distribution3D {
    Distribution<T> x;
    Distribution<T> y;
    Distribution<T> z;
};

}  // namespace rayx::design
