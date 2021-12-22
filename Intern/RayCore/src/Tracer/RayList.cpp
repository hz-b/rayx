#include "RayList.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>

namespace RAYX {

RayList::RayList() {
    // addVector();
}

RayList::~RayList() {}

void RayList::addVector() {
    if (m_rayList.size() > 0) {
        assert(((m_rayList.back()).size() == RAY_MAX_ELEMENTS_IN_VECTOR) &&
               "Size of last Ray vector is not equal to RAY_VECTOR_SIZE");
    }
    std::vector<Ray> newRayVector;
    m_rayList.push_back(newRayVector);
    m_rayList.back().reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
}

void RayList::insertVector(const std::vector<Ray>& inRayVector) {
    // cut inRayVector to RAY_VECTOR_SIZE big pieces and emplace back
    int numberOfVecs = inRayVector.size() / RAY_MAX_ELEMENTS_IN_VECTOR;
    if (inRayVector.size() % RAY_MAX_ELEMENTS_IN_VECTOR != 0) {
        numberOfVecs++;
    }

    // TODO(Jannis): doesn't work correctly for multiple light sources (leaves
    // gaps)
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