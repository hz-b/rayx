#include "Random.h"

#include <algorithm>
#include <random>

static std::mt19937 RNG;

namespace RAYX {

void fixSeed(uint32_t seed) { RNG.seed(seed); }

void randomSeed() { fixSeed((uint32_t)time(nullptr)); }

uint32_t randomUint() { return RNG(); }

double randomDouble() { return ((double)randomUint()) / std::mt19937::max(); }

int randomIntInRange(int a, int b) {
    int low = std::min(a, b);
    int high = std::max(a, b);
    return low + randomUint() % (high + 1 - low);
}

double randomDoubleInRange(double a, double b) {
    double low = std::min(a, b);
    double high = std::max(a, b);
    return low + randomDouble() * (high - low);
}

double randomNormal(double mean, double stddev) {
    std::normal_distribution<double> distr(mean, stddev);
    return distr(RNG);
}

}  // namespace RAYX
