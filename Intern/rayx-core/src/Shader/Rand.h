#pragma once

#include <cstdint>

#include "Core.h"

namespace RAYX {

using RandCounter = uint64_t;

/*
 * Title: "Squares: A Fast Counter-Based RNG"
 * Author: Bernard Widynski
 * Date: November 24, 2020
 * URL: https://arxiv.org/pdf/2004.06278.pdf
 */
// generates 64-Bit random integers
RAYX_FN_ACC uint64_t RAYX_API squares64(uint64_t& ctr);

// generates uniformly distributed doubles between 0 and 1 from one 64-Bit
// random integer
RAYX_FN_ACC double RAYX_API squaresDoubleRNG(uint64_t& ctr);

// creates (via the Box-Muller transform) a normal distributed double with mean
// mu and standard deviation sigma
RAYX_FN_ACC double RAYX_API squaresNormalRNG(uint64_t& ctr, double mu, double sigma);

struct Rand {
    Rand() noexcept {}

    Rand(const Rand&)            = delete;
    Rand(Rand&&)                 = default;
    Rand& operator=(const Rand&) = delete;
    Rand& operator=(Rand&&)      = default;

    RAYX_FN_ACC
    explicit Rand(const uint64_t ctr) noexcept : counter(ctr) {}

    RAYX_FN_ACC
    explicit Rand(const int rayIndex, const int numRaysTotal, const double randomSeed) noexcept {
        // ray specific "seed" for random numbers -> every ray has a different starting value for the counter that creates the random number
        const uint64_t MAX_UINT64      = ~(static_cast<uint64_t>(0));
        const double MAX_UINT64_DOUBLE = 18446744073709551616.0;
        uint64_t workerCounterNum      = MAX_UINT64 / static_cast<uint64_t>(numRaysTotal);
        counter                        = rayIndex * workerCounterNum + static_cast<uint64_t>(randomSeed * MAX_UINT64_DOUBLE);
    }

    RAYX_FN_ACC
    uint64_t randomInt() { return squares64(counter); }

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

    uint64_t counter;
};

}  // namespace RAYX
