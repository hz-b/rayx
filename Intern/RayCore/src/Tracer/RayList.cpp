#include "RayList.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "Debug.h"

namespace RAYX {

RayList::RayList() {}

RayList::~RayList() {}

/**
 * @brief insertVector function inserts a vector of Rays to the ray list.
 * It also fits rays from different light sources tightly one after another.
 * Intactness of inRayVector is not guaranteed.
 * @param inRayVector vector of Rays to be inserted
 */
void RayList::insertVector(std::vector<Ray>&& inRayVector) {
    // note that this can be optimized quite a bit.
    // But the previous "optimized" version had multiple memory unsafety issues
    // and was hence replaced.

    for (Ray r : inRayVector) {
        if (m_rayList.empty() ||
            m_rayList.back().size() == RAY_MAX_ELEMENTS_IN_VECTOR) {
            m_rayList.push_back({});
        }
        m_rayList.back().push_back(r);
    }
}

void RayList::clean() { m_rayList.clear(); }

std::list<std::vector<Ray>>::iterator RayList::begin() {
    return m_rayList.begin();
}
std::list<std::vector<Ray>>::iterator RayList::end() { return m_rayList.end(); }
std::vector<Ray> RayList::back() { return m_rayList.back(); }
std::size_t RayList::size() { return m_rayList.size(); }

int RayList::rayAmount() const {
    int amount = (m_rayList.size() - 1) * RAY_MAX_ELEMENTS_IN_VECTOR +
                 (m_rayList.back()).size();
    return amount;
}
}  // namespace RAYX
