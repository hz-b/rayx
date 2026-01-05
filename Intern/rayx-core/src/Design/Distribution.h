#pragma once

namespace rayx {

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
    shared_ref<DataSet<T>> values;
    std::optional<shared_ref<DataSet<double>>> weights;
    bool interpolate = false;
};

template <typename T>
using Distribution = std::variant<T, SeparateValues<T>, WhiteNoiseDistribution<T>, GaussianDistribution<T>, BakedDistribution<T>>;

template <typename T>
struct Cube {
    T x = T();
    T y = T();
    T z = T();
};

template <typename T>
struct Sphere {
    T radius = T();
};

template <typename T>
struct Rect {
    T x = T();
    T y = T();
};

}  // namespace rayx
