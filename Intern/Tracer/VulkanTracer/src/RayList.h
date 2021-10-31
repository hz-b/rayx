#pragma once
#pragma pack(16)

#include <list>
#include "Ray.h"

#define RAY_MAX_ELEMENTS_IN_VECTOR 262144
#define RAY_VECTOR_SIZE 16777216
#define RAY_DOUBLE_COUNT 13

class RayList {
public:
    //list of vectors
    RayList();
    ~RayList();
    //appends vector of Rays to the ray list
    void insertVector(void* location, size_t size);
    std::list<std::vector<Ray>>::iterator begin();
    std::list<std::vector<Ray>>::iterator end();
    std::vector<Ray> back();
    std::size_t size();
    int rayAmount() const;

private:
    //adds empty vector to the list
    std::list<std::vector<Ray>> m_rayList;
    void addVector();

};