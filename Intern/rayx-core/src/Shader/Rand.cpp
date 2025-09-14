#include "Rand.h"

#include <glm.hpp>

#include "Constants.h"

namespace RAYX {

constexpr uint64_t RNG_KEY = (uint64_t(0xc8e4fd15) << 32) | uint64_t(0x4ce32f6d);

RAYX_FN_ACC
uint64_t RAYX_API squares64(uint64_t& ctr) {
    uint64_t x, y, z, t;
    y = x = ctr * RNG_KEY;
    z     = y + RNG_KEY;
    ctr++;

    x = x * x + y;
    x = (x >> 32) | (x << 32); /* round 1 */
    x = x * x + z;
    x = (x >> 32) | (x << 32); /* round 2 */
    x = x * x + y;
    x = (x >> 32) | (x << 32); /* round 3 */
    t = x = (x * x + z);
    x     = (x >> 32) | (x << 32); /* round 4 */
    return t ^ ((x * x + y) >> 32);
}

RAYX_FN_ACC
double RAYX_API squaresDoubleRNG(uint64_t& ctr) {
    double a   = double(squares64(ctr));
    double div = double(uint64_t(0) - 1);
    return a / div;
}

RAYX_FN_ACC
double RAYX_API squaresNormalRNG(uint64_t& ctr, double mu, double sigma) {
    double U, V, R, Z;
    double two_pi = 2.0 * PI;

    U = squaresDoubleRNG(ctr);
    V = squaresDoubleRNG(ctr);
    R = squaresDoubleRNG(ctr);
    Z = glm::sqrt(-2.0 * glm::log(U));

    if (R < 0.5)
        Z *= glm::sin(two_pi * V);
    else
        Z *= glm::cos(two_pi * V);

    Z = Z * sigma + mu;

    return Z;
}

}  // namespace RAYX
