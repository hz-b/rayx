#pragma once

#include "Core.h"

namespace RAYX {

/*
 * Title: "Squares: A Fast Counter-Based RNG"
 * Author: Bernard Widynski
 * Date: November 24, 2020
 * URL: https://arxiv.org/pdf/2004.06278.pdf
 */

/// generates 64-Bit random integers
RAYX_FN_ACC uint64_t RAYX_API squares64(uint64_t& ctr);

/// generates uniformly distributed doubles between 0 and 1 from one 64-Bit
/// random integer
RAYX_FN_ACC double RAYX_API squaresDoubleRNG(uint64_t& ctr);

/// creates (via the Box-Muller transform) a normal distributed double with mean
/// mu and standard deviation sigma
RAYX_FN_ACC double RAYX_API squaresNormalRNG(uint64_t& ctr, double mu, double sigma);

class RAYX_API Rand {
  public:
    Rand() : m_ctr(0) {}
    RAYX_FN_ACC Rand(uint64_t seed) : m_ctr(seed) {}
    RAYX_FN_ACC Rand(int32_t rayId, int32_t numRays, double seed);
    Rand(const Rand&) = delete;
    Rand(Rand&&) = default;

    Rand& operator=(const Rand&) = delete;
    Rand& operator=(Rand&&) = default;

    RAYX_FN_ACC uint64_t ctr() const { return m_ctr; }

    /// generates 64-Bit random integers
    RAYX_FN_ACC uint64_t randomInt();

    /// generates integer in range
    RAYX_FN_ACC int randomIntInRange(int low, int high);

    /// samples the uniform distribution over the interval [min(a, b), max(a, b)].
    RAYX_FN_ACC double randomDoubleInRange(double a, double b);

    /// generates uniformly distributed doubles between 0 and 1 from one 64-Bit
    /// random integer
    RAYX_FN_ACC double randomDouble();

    /// creates (via the Box-Muller transform) a normal distributed double with mean
    /// mu and standard deviation sigma
    RAYX_FN_ACC double randomDoubleNormalDistributed(double mu, double sigma);

    // see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
    // we don't use std::normal_distribution, due to this:
    // https://stackoverflow.com/questions/38532927/why-gcc-and-msvc-stdnormal-distribution-are-different
    // `mu` is the mean, `sigma` is the standard deviation.
    RAYX_FN_ACC double randomNormal(double mu, double sigma);

  private:
    uint64_t m_ctr;
};

}  // namespace RAYX
