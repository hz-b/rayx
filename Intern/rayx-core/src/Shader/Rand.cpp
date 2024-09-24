#include "Rand.h"

#include <glm.h>

#include "Constants.h"

namespace {

constexpr uint64_t RNG_KEY = (uint64_t(0xc8e4fd15) << 32) | uint64_t(0x4ce32f6d);

}  // unnamed namespace

namespace RAYX {

RAYX_FN_ACC
uint64_t squares64(uint64_t& ctr) {
    uint64_t x, y, z, t;
    y = x = ctr * RNG_KEY;
    z = y + RNG_KEY;
    ctr++;

    x = x * x + y;
    x = (x >> 32) | (x << 32); /* round 1 */
    x = x * x + z;
    x = (x >> 32) | (x << 32); /* round 2 */
    x = x * x + y;
    x = (x >> 32) | (x << 32); /* round 3 */
    t = x = (x * x + z);
    x = (x >> 32) | (x << 32); /* round 4 */
    return t ^ ((x * x + y) >> 32);
}

RAYX_FN_ACC
double squaresDoubleRNG(uint64_t& ctr) {
    double a = double(squares64(ctr));
    double div = double(uint64_t(0) - 1);
    return a / div;
}

RAYX_FN_ACC
double squaresNormalRNG(uint64_t& ctr, double mu, double sigma) {
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

RAYX_FN_ACC
Rand::Rand(int32_t rayId, int32_t numRays, double seed) {
    // ray specific "seed" for random numbers -> every ray has a different starting value for the counter that creates the random number
    // TODO Random seeds should probably not be doubles! Casting MAX_UINT64 to double loses precision.
    const uint64_t MAX_UINT64 = ~(uint64_t(0));
    const double MAX_UINT64_DOUBLE = 18446744073709551616.0;
    uint64_t workerCounterNum = MAX_UINT64 / uint64_t(numRays);
    m_ctr = rayId * workerCounterNum + uint64_t(seed * MAX_UINT64_DOUBLE);
}

RAYX_FN_ACC
uint64_t Rand::randomInt() { return squares64(m_ctr); }

RAYX_FN_ACC int Rand::randomIntInRange(int low, int high) { return low + randomInt() % (high + 1 - low); }

RAYX_FN_ACC
double Rand::randomDouble() { return squaresDoubleRNG(m_ctr); }

RAYX_FN_ACC
double Rand::randomDoubleInRange(double a, double b) {
    double low = std::min(a, b);
    double high = std::max(a, b);
    return low + randomDouble() * (high - low);
}

RAYX_FN_ACC
double Rand::randomDoubleNormalDistributed(double mu, double sigma) { return squaresNormalRNG(m_ctr, mu, sigma); }

}  // namespace RAYX
