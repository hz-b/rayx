#include "RandomRays.h"
#include <cassert>
#include <cmath>
#include <random>

namespace RAYX
{

    // angles given and stored in rad
    RandomRays::RandomRays(int n, int low, int high)
        : LightSource(0, n, "Random rays", 0, 100, 0, { 0,0,0,0 }), m_low(low), m_high(high) {}

    RandomRays::~RandomRays()
    {
    }

    /**
     * creates floor(sqrt(numberOfRays)) **2 rays (a grid with as many rows as columns, eg amountOfRays=20 -> 4*4=16, rest (4 rays) same as first 4)
     * distributed evenly across width & height of source
     * returns list of rays
     */
    std::vector<Ray> RandomRays::getRays() {
        std::uniform_real_distribution<double> unif(m_low, m_high);
        std::default_random_engine re;

        int n = this->getNumberOfRays();
        std::vector<Ray> rayList;
        std::cout << "create " << n << " random rays " << std::endl;
        // fill the square with rmat1xrmat1 rays
        for (int i = 0; i < n; i++) {
            glm::dvec3 position = glm::dvec3(unif(re), unif(re), unif(re));

            glm::dvec3 direction = glm::dvec4(unif(re), unif(re), unif(re), unif(re));
            double weight = unif(re);
            double en = unif(re);
            Ray r = Ray(position, direction, en, weight);
            rayList.emplace_back(r);
        }
        return rayList;
    }

    void RandomRays::compareRays(std::vector<Ray*> input, std::vector<double> output) {
        std::list<double> diff;
        std::cout.precision(17);
        //double max = 0;
        for (int i = 0; i < this->getNumberOfRays(); i++) {
            Ray* r1 = input[i];
            double a1 = atan(r1->m_position.x) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT];
            diff.push_back(a1);
            double a2 = atan(r1->m_position.y) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT + 1];
            diff.push_back(a2);
            double a3 = atan(r1->m_position.z) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT + 2];
            diff.push_back(a3);
            double a5 = atan(r1->m_direction.x) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT + 4];
            diff.push_back(a5);
            double a6 = atan(r1->m_direction.y) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT + 5];
            diff.push_back(a6);
            double a7 = atan(r1->m_direction.z) - output[i * RAYCORE_RAY_DOUBLE_AMOUNT + 6];
            diff.push_back(a7);
        }
        diff.sort();
        std::cout << "max difference: " << diff.front() << " " << diff.back() << std::endl;
    }


} // namespace RAYX