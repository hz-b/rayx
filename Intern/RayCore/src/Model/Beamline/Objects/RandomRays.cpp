#include "RandomRays.h"

#include <cassert>
#include <cmath>
#include <random>

#include "Debug/Instrumentor.h"
#include "Tracer/Vulkan/VulkanTracer.h"

namespace RAYX {

/** creates random rays where each parameter is chosen uniformly from [low,
 * high] used for testing only as these rays do not make much sense
 */
RandomRays::RandomRays(int low, int high)
    : LightSource("Random rays",
                  EnergyDistribution(EnergyRange(100., 0.), true), 0, 0, 0,
                  {0, 0, 0, 0}),
      m_low(low),
      m_high(high) {}

RandomRays::~RandomRays() {}

/**
 * every parameter is chosen randomly
 * returns list of rays
 */
std::vector<Ray> RandomRays::getRays() {
    RAYX_PROFILE_FUNCTION();
    std::uniform_real_distribution<double> unif(m_low, m_high);
    std::default_random_engine re;

    int n = SimulationEnv::get().m_numOfRays;
    std::vector<Ray> rayList;
    std::cout << "[RandomRays]: create " << n << " random rays " << std::endl;
    // fill the square with rmat1xrmat1 rays
    for (int i = 0; i < n; i++) {
        glm::dvec3 position = glm::dvec3(unif(re), unif(re), unif(re));

        glm::dvec3 direction = glm::dvec3(unif(re), unif(re), unif(re));
        double weight = unif(re);
        double en = unif(re);
        glm::dvec4 stokes = glm::dvec4(unif(re), unif(re), unif(re), unif(re));
        Ray r = {position.x, position.y, position.z, weight, direction.x,
                        direction.y, direction.z, en, stokes.x, stokes.y,
                        stokes.z, stokes.w, 0.0, 0.0, 0.0, 0.0};
        rayList.emplace_back(r);
    }
    return rayList;
}

void RandomRays::compareRays(std::vector<Ray*> input,
                             std::vector<double> output) {
    std::list<double> diff;
    std::cout.precision(17);
    // double max = 0;
    for (int i = 0; i < SimulationEnv::get().m_numOfRays; i++) {
        Ray* r1 = input[i];
        double a1 =
            atan(r1->m_position.x) - output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT];
        diff.push_back(a1);
        double a2 = atan(r1->m_position.y) -
                    output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT + 1];
        diff.push_back(a2);
        double a3 = atan(r1->m_position.z) -
                    output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT + 2];
        diff.push_back(a3);
        double a5 = atan(r1->m_direction.x) -
                    output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT + 4];
        diff.push_back(a5);
        double a6 = atan(r1->m_direction.y) -
                    output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT + 5];
        diff.push_back(a6);
        double a7 = atan(r1->m_direction.z) -
                    output[i * VULKANTRACER_RAY_DOUBLE_AMOUNT + 6];
        diff.push_back(a7);
    }
    diff.sort();
    std::cout << "[RandomRays]: max difference: " << diff.front() << " "
              << diff.back() << std::endl;
}

}  // namespace RAYX