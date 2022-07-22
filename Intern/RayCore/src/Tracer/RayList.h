#pragma once
#pragma pack(16)

#include <list>
#include <vector>

#include "Ray.h"

// Optimal size for a ray vector (only tested on one system). Higher numbers
// will sometimes lead to bad allocs.
#define RAY_VECTOR_SIZE 16777216
// Doubles per ray
#define RAY_DOUBLE_COUNT 16
// Rays per vector
#define RAY_MAX_ELEMENTS_IN_VECTOR \
    (RAY_VECTOR_SIZE / (sizeof(double) * RAY_DOUBLE_COUNT))

namespace RAYX {

struct RayListIter {
    std::list<std::vector<Ray>>::iterator m_iter;  // iterator over list
    size_t m_offset;                               // index within std::vector

    bool operator==(const RayListIter& o) const;
    bool operator!=(const RayListIter& o) const;
    void operator++();
    Ray& operator*();
};

struct ConstRayListIter {
    std::list<std::vector<Ray>>::const_iterator m_iter;
    size_t m_offset;

    bool operator==(const ConstRayListIter&) const;
    bool operator!=(const ConstRayListIter&) const;
    void operator++();
    const Ray& operator*();
};

class RayList {
  public:
    // list of vectors
    RayList();
    ~RayList();
    // appends vector of Rays to the ray list
    void insertVector(const std::vector<Ray>& inRayVector);
    void push(Ray);
    void clean();

    RayListIter begin();
    RayListIter end();

    ConstRayListIter cbegin() const;
    ConstRayListIter cend() const;

    int rayAmount() const;

    std::list<std::vector<Ray>>
        m_rayList;  // TODO(Rudi) rename, writing rayList.m_rayList is
                    // confusing. Typical contains name this .data()

    // a typical filter operator, receiving a function f of type Ray& -> bool
    // (aka F), this returns a new RayList returning only those who satisfy the
    // constraint f.
    template <typename F>
    inline RayList filter(F f) const {
        RayList out;
        for (auto it = cbegin(); it != cend(); ++it) {
            auto r = *it;
            if (f(r)) {
                out.push(r);
            }
        }
        return out;
    }
};

}  // namespace RAYX
