#include "RayList.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>

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
    // check if last vector is full, if not fill it
    size_t remainingSpace = 0;
    if ((m_rayList.back().size() > 0) && (m_rayList.back().size() < RAY_MAX_ELEMENTS_IN_VECTOR)) {
        remainingSpace = RAY_MAX_ELEMENTS_IN_VECTOR - m_rayList.back().size();
        if (remainingSpace > 0) {
            // fill remaining space
            m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
            m_rayList.back().insert(m_rayList.back().end(), inRayVector.begin(),
                                    inRayVector.begin() + remainingSpace);
            // erase from vector
            inRayVector.erase(inRayVector.begin(),
                              inRayVector.begin() + remainingSpace);
        }
    }

    // cut inRayVector to RAY_VECTOR_SIZE big pieces and emplace back
    int numberOfVecs = inRayVector.size() / RAY_MAX_ELEMENTS_IN_VECTOR;
    if (inRayVector.size() % RAY_MAX_ELEMENTS_IN_VECTOR != 0) {
        numberOfVecs++;
    }
    for (int i = 0; i < numberOfVecs; i++) {
        if (i == numberOfVecs - 1) {
            // last vector
            m_rayList.emplace_back(
                inRayVector.begin() + i * RAY_MAX_ELEMENTS_IN_VECTOR,
                inRayVector.end());
        } else {
            m_rayList.emplace_back(
                inRayVector.begin() + i * RAY_MAX_ELEMENTS_IN_VECTOR,
                inRayVector.begin() + (i + 1) * RAY_MAX_ELEMENTS_IN_VECTOR);
        }
    }
}

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