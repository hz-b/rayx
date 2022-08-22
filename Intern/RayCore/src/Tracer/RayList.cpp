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
    if (m_data.empty() || m_data.back().size() == RAY_MAX_ELEMENTS_IN_VECTOR) {
        std::vector<Ray> v;
        v.reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
        m_data.emplace_back(v);
    }
    m_data.back().push_back(r);
}

void RayList::append(const RayList& other) {
    for (auto& vec : other.m_data) {
        insertVector(vec);
    }
}

/**
 * @brief Get window into raw data of ray list (list of vectors of rays).
 * @return const ray list data
 */
const std::list<std::vector<Ray>>& RayList::getData() const {
    return m_data;
}

/**
 * @brief rayAmount returns the number of rays in the ray list.
 * @return number of rays in the ray list.
 */
size_t RayList::rayAmount() const {
    if (m_data.empty()) {
        return 0;
    } else {
        auto back = m_data.back();
        int amount =
            back.size() + (m_data.size() - 1) * RAY_MAX_ELEMENTS_IN_VECTOR;
        return amount;
    }
}

void RayList::clean() { m_data.clear(); }

// Ray& at(size_t index);
const Ray& RayList::at(size_t index) const {
    size_t vecIndex = index / RAY_MAX_ELEMENTS_IN_VECTOR;
    size_t elemIndex = index % RAY_MAX_ELEMENTS_IN_VECTOR;
    auto listIter = m_data.begin();
    std::advance(listIter, vecIndex);
    return (*listIter)[elemIndex];
}

// Ray& operator[](size_t index);
const Ray& RayList::operator[](size_t index) const { return at(index); }

RayListIter RayList::begin() {
    return {.m_iter = m_data.begin(), .m_offset = 0};
}
RayListIter RayList::end() { return {.m_iter = m_data.end(), .m_offset = 0}; }

ConstRayListIter RayList::begin() const {
    return {.m_iter = m_data.cbegin(), .m_offset = 0};
}
ConstRayListIter RayList::end() const {
    return {.m_iter = m_data.cend(), .m_offset = 0};
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
