#include "RandomRays.h"

#include <cmath>
#include <random>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "RandomRays.h"

namespace RAYX {

/** creates random rays where each parameter is chosen uniformly from [low,
 * high] used for testing only as these rays do not make much sense
 */
RandomRays::RandomRays(int low, int high, int numberOfRays)
    : LightSource("Random rays", numberOfRays, EnergyDistribution(EnergyRange(100., 0.), true), 0, 0, 0, {0, 0, 0, 0}, 0, 0, 0, 0, 0),
      m_low(low),
      m_high(high) {}

RandomRays::~RandomRays() = default;

/**
 * every parameter is chosen randomly
 * returns list of rays
 */
std::vector<Ray> RandomRays::getRays() const {
    RAYX_PROFILE_FUNCTION();
    std::uniform_real_distribution<double> unif(m_low, m_high);
    std::default_random_engine re;

    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    RAYX_VERB << "create " << m_numberOfRays << " random rays ";
    // fill the square with rmat1xrmat1 rays
    for (uint32_t i = 0; i < m_numberOfRays; i++) {
        glm::dvec3 position = glm::dvec3(unif(re), unif(re), unif(re));

        glm::dvec3 direction = glm::dvec3(unif(re), unif(re), unif(re));
        double weight = unif(re);
        double en = unif(re);
        glm::dvec4 stokes = glm::dvec4(unif(re), unif(re), unif(re), unif(re));
        Ray r = {position, weight, direction, en, stokes, 0.0, 0.0, 0.0, 0.0};
        rayList.push_back(r);
    }
    return rayList;
}

}  // namespace RAYX
