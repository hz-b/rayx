#pragma once
#include "Core.h"

// All C++ randomness should be using this module.
// Otherwise set-seeding tests will not work.
// The seed for the shader should also be generated from this module.

namespace RAYX {

// fixed-seed tests operate on this seed.
const uint32_t FIXED_SEED = 42;

// Sets the current `seed` to `x`.
// This resets the internal RNG state, so any future values returned by this random generator are deterministic - only based on the seed x.
// In order to make your function RNG-deterministic, you need to call fixSeed once at the beginning of your function.
// This function is used for fixed-seed testing.
void RAYX_API fixSeed(uint32_t x);

// sets the seed "randomly", depending on system time.
void RAYX_API randomSeed();

// samples an integer from the uniform integer distribution over the interval [0, 2^32[
uint32_t randomUint();

// samples a double from the uniform distribution over the interval [0, 1]
double RAYX_API randomDouble();

// samples an integer from the uniform distribution over the interval [min(a, b), max(a, b)]
int randomIntInRange(int a, int b);

// samples a double from the uniform distribution over the interval [min(a, b), max(a, b)]
double RAYX_API randomDoubleInRange(double a, double b);

// samples a value from the given normal distribution.
// `mean` is evidently the mean of the distribution, while `stddev` is the standard deviation (often written as sigma).
double RAYX_API randomNormal(double mean, double stddev);

}  // namespace RAYX
