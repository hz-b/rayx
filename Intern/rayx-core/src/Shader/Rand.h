#pragma once

#include <cstdint>

#include "Core.h"

namespace RAYX {

/// Internal type for the random counter-based RNG
using RandCounter = RandCounter;

/*
 * Title: "Squares: A Fast Counter-Based RNG"
 * Author: Bernard Widynski
 * Date: November 24, 2020
 * URL: https://arxiv.org/pdf/2004.06278.pdf
 */
// generates 64-Bit random integers
RAYX_FN_ACC RandCounter RAYX_API squares64(RandCounter& ctr);

// generates uniformly distributed doubles between 0 and 1 from one 64-Bit
// random integer
RAYX_FN_ACC double RAYX_API squaresDoubleRNG(RandCounter& ctr);

// creates (via the Box-Muller transform) a normal distributed double with mean
// mu and standard deviation sigma
RAYX_FN_ACC double RAYX_API squaresNormalRNG(RandCounter& ctr, double mu, double sigma);

struct Rand {
    Rand() noexcept {}

    Rand(const Rand&)            = delete;
    Rand(Rand&&)                 = default;
    Rand& operator=(const Rand&) = delete;
    Rand& operator=(Rand&&)      = default;

    RAYX_FN_ACC
    explicit Rand(const RandCounter ctr) noexcept : counter(ctr) {}

    RAYX_FN_ACC
    explicit Rand(const int rayPathIndex, const int numRaysTotal, const double randomSeed) noexcept {
        // ray specific "seed" for random numbers -> every ray has a different starting value for the counter that creates the random number
        const RandCounter MAX_UINT64   = ~(static_cast<RandCounter>(0));
        const double MAX_UINT64_DOUBLE = 18446744073709551616.0;
        RandCounter workerCounterNum   = MAX_UINT64 / static_cast<RandCounter>(numRaysTotal);
        counter                        = rayPathIndex * workerCounterNum + static_cast<RandCounter>(randomSeed * MAX_UINT64_DOUBLE);

        // TODO: replace above with below and test if it works correctly
        // constexpr auto MAX_UINT64 = std::numeric_limits<RandCounter>::max();
        // constexpr auto MAX_UINT64_DOUBLE = static_cast<double>(MAX_UINT64);
        // const auto workerCounterNum =  MAX_UINT64 / static_cast<RandCounter>(numRaysTotal);
        // const auto randomPhase = static_cast<RandCounter>(randomSeed * MAX_UINT64_DOUBLE);
        // counter = rayPathIndex * workerCounterNum + randomPhase;
    }

    RAYX_FN_ACC
    uint64_t randomInt() { return squares64(counter); }

    // TODO: review this function. does the combination of int and uint work as intended?
    RAYX_FN_ACC
    int randomIntInRange(const int min_inclusive, const int max_exclusive) {
        return min_inclusive + squares64(counter) % (max_exclusive + 1 - min_inclusive);
    }

    RAYX_FN_ACC
    double randomDouble() { return squaresDoubleRNG(counter); }

    RAYX_FN_ACC
    double randomDoubleInRange(const double min, const double max) { return min + randomDouble() * (max - min); }

    RAYX_FN_ACC
    double randomDoubleNormalDistributed(double mu, double sigma) { return squaresNormalRNG(counter, mu, sigma); }

    RandCounter counter;
};

}  // namespace RAYX
