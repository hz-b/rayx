#pragma once

#include <list>
#include "Ray.h"

#define RAY_MAX_ELEMENTS_IN_VECTOR 1048576
#define RAY_VECTOR_SIZE 67108864
#define RAY_DOUBLE_COUNT 8

class RayList{
    private:
        //adds empty vector to the list
        std::list<std::vector<Ray>> m_rayList;
        void addVector();         
        

    public:
        RayList();
        ~RayList();

        //appends vector of Rays to the ray list
        void insertVector(std::vector<Ray> input);
        std::list<std::vector<Ray>>::iterator begin();
        std::list<std::vector<Ray>>::iterator end();
        std::size_t size();
        int rayAmount();

    };