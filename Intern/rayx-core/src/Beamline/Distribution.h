#pragma once

namespace rayx {

template <typename T>
struct RangeSampler {
    T halfRange                   = T();
    std::optional<int> numSamples;
    bool montecarlo = true;
};

template <typename T>
struct FixedValueDistribution {
    T value = T();
};

template <typename T>
struct UniformDistribution {
    T center = T();
    RangeSampler sampler;
};

template <typename T>
struct BrownianDistribution {
    T center            = T();
    T standardDeviation = T();
    RangeSampler sampler;
};

// TODO: conssider adding FWHM representation
template <typename T>
struct GaussianDistribution {
    T mean = T()
    T standardDeviation = T();
    std::optional<RangeSampler> sampler;
};

template <typename T>
struct BakedDistribution {
    std::vector<T> samples = T();
    std::optional<std::vector<double>> weights;
    bool interpolate = false;
};

template <typename T>
using Distribution        = std::variant<FixedValueDistribution<T>, UniformDistribution<T>, BrownianDistribution<T>, GaussianDistribution<T>, BakedDistribution<T>>;

namespace defaults {
constexpr PhotonEnergyDistribution photonEnergy = PhotonEnergyFixedValueDistribution{ .value = 1.0 /* infrared */ };
}

template <typename T>
struct CircularDistribution {
    Distribution<T> radiusDistribution;
};

template <typename T>
struct RectangularDistribution {
    Distribution<T> horizontalDistribution;
    Distribution<T> verticalDistribution;
};

template <typename T>
using AreaDistribution        = std::variant<CircularDistribution<T>, RectangularDistribution<T>>;

template <typename T>
struct SphericalDistribution {
    Distribution<T> radiusDistribution;
};

template <typename T>
struct CylindricalDistribution {
    Distribution<T> radiusDistribution;
    Distribution<T> depthDistribution;
};

template <typename T>
struct CuboidalDistribution {
    Distribution<T> horizontalDistribution;
    Distribution<T> verticalDistribution;
    Distribution<T> depthDistribution;
};

template <typename T>
using VolumetricDistribution       = std::variant<CuboidalDistribution<T>, CylindricalDistribution<T>, SphericalDistribution<T>>;

}  // namespace rayx
