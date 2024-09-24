#include "Random.h"

#include "Shader/Rand.h"
#include "Shader/Constants.h"

namespace RAYX {

static Rand rand;

void fixSeed(uint32_t seed) { rand = Rand(seed); }

void randomSeed() { fixSeed((uint32_t)time(nullptr)); }

uint32_t randomUint() { return rand.randomInt(); }

double randomDouble() { return rand.randomDouble(); }

int randomIntInRange(int a, int b) {
    int low = std::min(a, b);
    int high = std::max(a, b);
    return low + randomUint() % (high + 1 - low);
}

// samples the uniform distribution over the interval [min(a, b), max(a, b)].
double randomDoubleInRange(double a, double b) {
    double low = std::min(a, b);
    double high = std::max(a, b);
    return low + rand.randomDouble() * (high - low);
}

double randomNormal(double mu, double sigma) {
    return rand.randomDoubleNormalDistributed(mu, sigma);
}

}  // namespace RAYX
