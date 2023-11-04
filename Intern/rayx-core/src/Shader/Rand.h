/*
 * Title: "Squares: A Fast Counter-Based RNG"
 * Author: Bernard Widynski
 * Date: November 24, 2020
 * URL: https://arxiv.org/pdf/2004.06278.pdf
 */
// generates 64-Bit random integers
uint64_t RAYX_API squares64(RAYX_INOUT(uint64_t) ctr);

// generates uniformly distributed doubles between 0 and 1 from one 64-Bit
// random integer
double RAYX_API squaresDoubleRNG(RAYX_INOUT(uint64_t) ctr);

// creates (via the Box-Muller transform) a normal distributed double with mean
// mu and standard deviation sigma
double RAYX_API squaresNormalRNG(RAYX_INOUT(uint64_t) ctr, double mu, double sigma);
