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

/** RayList is a list of vectors of rays. Each vector has a fixed size.
 *   When a vector is full, a new vector is created. The reason for this
 *   design is to avoid memory allocation errors.
 */
class RayList {
  public:
    RayList();
    ~RayList();
    const std::list<std::vector<Ray>>& getData() const;
    void insertVector(const std::vector<Ray>& inRayVector);
    void push(Ray);
    void append(const RayList& other);
    size_t rayAmount() const;
    void clean();

    // Ray& at(size_t index);
    const Ray& at(size_t index) const;
    // Ray& operator[](size_t index);
    const Ray& operator[](size_t index) const;

    RayListIter begin();
    RayListIter end();

    ConstRayListIter begin() const;
    ConstRayListIter end() const;

    // a typical filter operator, receiving a function f of type Ray& -> bool
    // (aka F), this returns a new RayList returning only those who satisfy the
    // constraint f.
    template <typename F>
    inline RayList filter(F f) const {
        RayList out;
        for (auto r : *this) {
            if (f(r)) {
                out.push(r);
            }
        }
        return out;
    }

  private:
    std::list<std::vector<Ray>> m_data;
};

}  // namespace RAYX
