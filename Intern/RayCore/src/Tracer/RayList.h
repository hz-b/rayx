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

class RayList {
  public:
    // list of vectors
    RayList();
    ~RayList();
    // appends vector of Rays to the ray list
    void insertVector(const std::vector<Ray>& inRayVector);
    void clean();
    std::list<std::vector<Ray>>::iterator begin();
    std::list<std::vector<Ray>>::iterator end();
    std::vector<Ray> back();
    std::size_t size();
    int rayAmount() const;

  private:
    // adds empty vector to the list
    std::list<std::vector<Ray>> m_rayList;
};
}  // namespace RAYX
