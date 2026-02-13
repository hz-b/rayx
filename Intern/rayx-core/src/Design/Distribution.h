#pragma once

#include "Angle.h"
#include "Area.h"
#include "Coating.h"
#include "Curvature.h"
#include "Material.h"
#include "Polarization.h"
#include "Property.h"
#include "Rotation.h"

namespace rayx {

template <typename T>
struct SeparateValues {
    constexpr SeparateValues(const T center, const T range, const int numValues) : m_center(center), m_range(range) { this->numValues(numValues); }

    RAYX_NESTED_PROPERTY(SeparateValues, T, center);
    RAYX_NESTED_PROPERTY(SeparateValues, T, range);
    RAYX_VALIDATED_PROPERTY(SeparateValues, int, numValues, detail::validateGreaterZero);
};

template <typename T>
struct WhiteNoiseDistribution {
    constexpr WhiteNoiseDistribution(const T center, const T range) : m_center(center), m_range(range) {}

    RAYX_NESTED_PROPERTY(WhiteNoiseDistribution, T, center);
    RAYX_NESTED_PROPERTY(WhiteNoiseDistribution, T, range);
};

template <typename T>
struct GaussianDistribution {
    constexpr GaussianDistribution(const T mean, const double standardDeviation) : m_mean(mean) { this->standardDeviation(standardDeviation); }

    RAYX_NESTED_PROPERTY(GaussianDistribution, T, mean);
    // standard deviation is treated as the same type as mean
    RAYX_VALIDATED_PROPERTY(GaussianDistribution, double, standardDeviation, detail::validateGreaterEqualZero);
};

template <typename T>
struct BakedDistribution {
    constexpr BakedDistribution(std::vector<T> values, std::optional<std::vector<T>> weights = std::nullopt) {
        this->valuesAndWeights(values, weights);
    }

    const std::vector<T>& values() const { return m_values; }
    const std::optional<std::vector<T>>& weights() const { return m_weights; }

    constexpr BakedDistribution& valuesAndWeights(std::vector<T> value, std::optional<std::vector<T>> weights = std::nullopt) {
        if (weights) detail::validateVectorSizesEqual("BakedDistribution", "values", value, "weights", *weights);
        m_values  = value;
        m_weights = weights;
        return *this;
    }

    RAYX_PROPERTY(BakedDistribution, bool, interpolate) = false;

  private:
    std::vector<T> m_values;
    std::optional<std::vector<T>> m_weights;
};

template <typename T>
using Distribution = std::variant<T, SeparateValues<T>, WhiteNoiseDistribution<T>, GaussianDistribution<T>, std::shared_ptr<BakedDistribution<T>>>;

template <typename T>
struct Distribution2D {
    constexpr Distribution2D() = default;
    constexpr Distribution2D(const Distribution<T> horizontal, const Distribution<T> vertical) : m_horizontal(horizontal), m_vertical(vertical) {}

    RAYX_NESTED_PROPERTY(Distribution2D, Distribution<T>, horizontal);
    RAYX_NESTED_PROPERTY(Distribution2D, Distribution<T>, vertical);
};

template <typename T>
struct Distribution3D {
    constexpr Distribution3D() = default;
    constexpr Distribution3D(const Distribution<T> horizontal, const Distribution<T> vertical, const Distribution<T> depth)
        : m_horizontal(horizontal), m_vertical(vertical), m_depth(depth) {}

    RAYX_NESTED_PROPERTY(Distribution3D, Distribution<T>, horizontal);
    RAYX_NESTED_PROPERTY(Distribution3D, Distribution<T>, vertical);
    RAYX_NESTED_PROPERTY(Distribution3D, Distribution<T>, depth);
};

}  // namespace rayx
