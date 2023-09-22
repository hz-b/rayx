# Pseudo Random Number Generators (PRNGs) on the GPU

For RAYX we looked a little to to find a good and performant pseudo random number generator. We decided to use Squares RNG, which is counter based and utilizes a version of the Middle Square Weyl Sequence. We tested the method with the TestU01 bigcrush test with different seeds and it passed all of them.

[Squares: A Fast Counter-Based RNG](https://arxiv.org/pdf/2004.06278.pdf)

[TestU01: A C Library for Empirical Testing of
Random Number Generators](https://www.iro.umontreal.ca/~lecuyer/myftp/papers/testu01.pdf)

We added a few more methods for creating random numbers with more variety. These methods are:
- `uint64_t squares64RNG(inout uint64_t ctr)`, which generates 64-Bit random integers from two 32-Bit random integers
- `double squaresDoubleRNG(inout uint64_t ctr)`, which generates uniformly distributed doubles between 0 and 1 from one 64-Bit random integer
- `double squaresNormalRNG(inout uint64_t ctr, double mu, double sigma)`, which creates (via the Box-Muller transform) a normal distributed double with mean `mu` and standard deviation `sigma`. This takes three random doulbes, which takes six 32-Bit integers.

[Box–Muller transform](https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform?oldformat=true)