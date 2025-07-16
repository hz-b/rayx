#pragma once

#include "Ray.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
inline int getRecordIndex(const int pathId, const int bounceId, const int maxEvents) { return pathId * maxEvents + bounceId; }

RAYX_FN_ACC
inline void recordEvent(Ray* __restrict output, Ray& __restrict ray, const int recordIndex) {
    // TODO: this code is currently disabled to ensure all tests run with the changes that were made. in the next commit this code will be enabled and
    // seeded will be override, due to changes in the electric field of written rays

    // we use a temporary here, to avoid copying the whole ray when writing the ray with the correct field value
    // const auto tmpField = ray.m_field;
    // ray.m_field = advanceElectricField(ray.m_field, energyToWaveLength(ray.m_energy), ray.m_pathLength, 1.0);
    output[recordIndex] = ray;
    // ray.m_field = tmpField;
}

}  // namespace RAYX
