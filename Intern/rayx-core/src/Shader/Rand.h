#pragma once

#include "Core.h"

namespace RAYX {

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

}  // namespace RAYX
