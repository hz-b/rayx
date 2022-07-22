#include "RayList.h"

#include <cstring>
#include <iostream>

#include "Debug.h"

namespace RAYX {

RayList::RayList() = default;

RayList::~RayList() = default;

/**
 * @brief insertVector function inserts a vector of Rays to the ray list.
 * It also fits rays from different light sources tightly one after another.
 * Intactness of inRayVector is not guaranteed.
 * @param inRayVector vector of Rays to be inserted
 */
void RayList::insertVector(const std::vector<Ray>& inRayVector) {
    // note that this can be optimized quite a bit.
    // But the previous "optimized" version had multiple memory unsafety issues
    // and was hence replaced.

    for (Ray r : inRayVector) {
        push(r);
    }
}

void RayList::push(Ray r) {
    if (m_data.empty() ||
        m_data.back().size() == RAY_MAX_ELEMENTS_IN_VECTOR) {
        m_data.emplace_back();
    }
    m_data.back().push_back(r);
}

void RayList::clean() { m_data.clear(); }

RayListIter RayList::begin() {
    return {.m_iter = m_data.begin(), .m_offset = 0};
}
RayListIter RayList::end() {
    return {.m_iter = m_data.end(), .m_offset = 0};
}

ConstRayListIter RayList::cbegin() const {
    return {.m_iter = m_data.cbegin(), .m_offset = 0};
}
ConstRayListIter RayList::cend() const {
    return {.m_iter = m_data.cend(), .m_offset = 0};
}

int RayList::rayAmount() const {
    int amount = (int)((m_data.size() - 1) * RAY_MAX_ELEMENTS_IN_VECTOR +
                       (m_data.back()).size());
    return amount;
}

// mutable RayListIter
bool RayListIter::operator==(const RayListIter& o) const {
    return m_iter == o.m_iter && m_offset == o.m_offset;
}
bool RayListIter::operator!=(const RayListIter& o) const {
    return !(*this == o);
}
void RayListIter::operator++() {
    m_offset++;
    if (m_offset >= m_iter->size()) {
        m_iter++;
        m_offset = 0;
    }
}
Ray& RayListIter::operator*() { return (*m_iter)[m_offset]; }

// ConstRayListIter
bool ConstRayListIter::operator==(const ConstRayListIter& o) const {
    return m_iter == o.m_iter && m_offset == o.m_offset;
}
bool ConstRayListIter::operator!=(const ConstRayListIter& o) const {
	return !(*this == o);
}
void ConstRayListIter::operator++() {
    m_offset++;
    if (m_offset >= m_iter->size()) {
        m_iter++;
        m_offset = 0;
    }
}
const Ray& ConstRayListIter::operator*() { return (*m_iter)[m_offset]; }

}  // namespace RAYX
