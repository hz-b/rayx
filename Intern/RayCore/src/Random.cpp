#include "Random.h"

#include <algorithm>
#include <random>

#include "Constants.h"

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

// see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
// we don't use std::normal_distribution, due to this:
// https://stackoverflow.com/questions/38532927/why-gcc-and-msvc-stdnormal-distribution-are-different
double randomNormal(double mu, double sigma) {
    constexpr double epsilon = std::numeric_limits<double>::epsilon();
    const double two_pi = 2.0 * PI;

    // create two random numbers, make sure u1 is greater than epsilon
    double u1, u2;
    do {
        u1 = randomDouble();
    } while (u1 <= epsilon);
    u2 = randomDouble();

    // compute z0 and z1
    auto mag = sigma * sqrt(-2.0 * log(u1));
    auto z0 = mag * cos(two_pi * u2) + mu;

    return z0;
}

}  // namespace RAYX
