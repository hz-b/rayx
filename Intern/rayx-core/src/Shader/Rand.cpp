#include "Rand.h"
#include "Approx.h"

const uint64_t RNG_KEY = (uint64_t(0xc8e4fd15) << 32) | uint64_t(0x4ce32f6d);

uint64_t RAYX_API squares64(RAYX_INOUT(uint64_t) ctr) {
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

double RAYX_API squaresDoubleRNG(RAYX_INOUT(uint64_t) ctr) {
    double a = double(squares64(ctr));
    double div = double(uint64_t(0) - 1);
    return a / div;
}

double RAYX_API squaresNormalRNG(RAYX_INOUT(uint64_t) ctr, double mu, double sigma) {
    double U, V, R, Z;
    double two_pi = 2.0 * PI;

    U = squaresDoubleRNG(ctr);
    V = squaresDoubleRNG(ctr);
    R = squaresDoubleRNG(ctr);
    Z = sqrt(-2.0 * r8_log(U));

    if (R < 0.5)
        Z *= r8_sin(two_pi * V);
    else
        Z *= r8_cos(two_pi * V);

    Z = Z * sigma + mu;

    return Z;
}
