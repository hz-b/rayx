#pragma once

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
