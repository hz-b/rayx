#pragma once
#pragma pack(16)

#include <list>
#include "Ray.h"

#define RAY_MAX_ELEMENTS_IN_VECTOR 262144
#define RAY_VECTOR_SIZE 16777216
#define RAY_DOUBLE_COUNT 8

class RayList {


public:
    RayList();
    ~RayList();

    //appends vector of Rays to the ray list
    void insertVector(std::vector<Ray> input);
    std::list<std::vector<Ray>>::iterator begin();
    std::list<std::vector<Ray>>::iterator end();
    std::size_t size();
    int rayAmount();

private:
    //adds empty vector to the list
    std::list<std::vector<Ray>> m_rayList;
    void addVector();

};