#pragma once
#include "Core.h"

// All C++ randomness should be using this module.
// Otherwise set-seeding tests will not work.
// The seed for the shader should also be generated from this module.

namespace RAYX {

// fixed-seed tests operate on this seed.
const uint32_t FIXED_SEED = 42;

// used for fixed-seed testing
void RAYX_API fixSeed(uint32_t x);

// sets the seed "randomly", depending on system time.
void RAYX_API randomSeed();

// in [0, 2^32[
uint32_t randomUint();

// is in [0, 1].
double RAYX_API randomDouble();

// is in [min(a, b), max(a, b)]
int randomIntInRange(int a, int b);

// is in [min(a, b), max(a, b)]
double RAYX_API randomDoubleInRange(double a, double b);

// returns a value from the given normal distribution.
double RAYX_API randomNormal(double mean, double stddev);

}  // namespace RAYX
