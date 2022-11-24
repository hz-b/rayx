#pragma once

// All C++ randomness should be using this module.
// Otherwise set-seeding tests will not work.
// The seed for the shader should also be generated from this module.

namespace RAYX {

// fixed-seed tests operate on this seed.
const uint32_t FIXED_SEED = 42;

// used for fixed-seed testing
void fixSeed(uint32_t x);

// sets the seed "randomly", depending on system time.
void randomSeed();

// in [0, 2^32[
uint32_t randomUint();

// is in [0, 1].
double randomDouble();

// is in [min(a, b), max(a, b)]
int randomIntInRange(int a, int b);

// is in [min(a, b), max(a, b)]
double randomDoubleInRange(double a, double b);

// returns a value from the given normal distribution.
double randomNormal(double mean, double stddev);

}  // namespace RAYX
