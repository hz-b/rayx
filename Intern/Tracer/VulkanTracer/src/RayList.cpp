#include "RayList.h"
#include <cassert>
#include <cmath>
#include <cstring>

RayList::RayList() {
    //addVector();
}

RayList::~RayList() {}

void RayList::addVector() {
    //std::cout << "add vector: start" << std::endl;
    if (m_rayList.size() > 0) {

        //std::cout<<"addVector: last vector size= "<< m_rayList.back().size() <<std::endl;    
        assert(("Size of last Ray vector is not equal to RAY_VECTOR_SIZE", (m_rayList.back()).size() == RAY_MAX_ELEMENTS_IN_VECTOR));

    }
    std::vector<Ray> newRayVector;
    //std::cout<<"add vector: capacity: "<<newRayVector.capacity()<<std::endl;
    m_rayList.push_back(newRayVector);
    //std::cout<<"add vector: reserving"<<std::endl;
    m_rayList.back().reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
    //std::cout << "add vector: end" << std::endl;
}

void RayList::insertVector(void* location, size_t inputSize) {
    //std::cout<<"insert vector: start"<<std::endl;
    std::vector<Ray> input;
    size_t lastVectorSize = 0;
    //if the last vector of the list is full, we can just append
    //std::cout<<"RayList size= "<< m_rayList.size() <<std::endl;
    //std::cout << "input size= " << inputSize << std::endl;
    if (m_rayList.size() == 0) {
        addVector();
    }
    size_t remainingBytes = inputSize * RAY_DOUBLE_COUNT * sizeof(double);
    while (remainingBytes != 0) {
        //std::cout << "insertVector: remainingBytes: " << remainingBytes << std::endl;
        size_t freeBytes = RAY_VECTOR_SIZE - (m_rayList.back().size() * RAY_DOUBLE_COUNT * sizeof(double));
        //std::cout << "insertVector: freeBytes: " << freeBytes << std::endl;

        //std::cout << "insertVector: rayList.size: " << m_rayList.size() << std::endl;

        if (freeBytes == 0) {
            addVector();
            freeBytes = RAY_VECTOR_SIZE;
        }
        size_t bytesToCopy = std::min(remainingBytes, freeBytes);
        //std::cout << "insertVector: bytesToCopy: " << bytesToCopy << std::endl;
        memcpy((m_rayList.back().data() + m_rayList.back().size()), (char*)(location + (inputSize * RAY_DOUBLE_COUNT * sizeof(double) - remainingBytes)), bytesToCopy);
        (m_rayList.back()).resize(m_rayList.back().size() + (bytesToCopy / (RAY_DOUBLE_COUNT * sizeof(double))));
        //std::cout << "insertVector: m_rayList.back().size(): " << m_rayList.back().size() << std::endl;
        //std::cout << "insertVector: sample ray: " << m_rayList.back()[0].getxDir() << std::endl;
        remainingBytes -= bytesToCopy;
    }

}

std::list<std::vector<Ray>>::iterator RayList::begin() {
    return m_rayList.begin();
}

std::list<std::vector<Ray>>::iterator RayList::end() {
    return m_rayList.end();
}
std::vector<Ray> RayList::back() {
    return m_rayList.back();
}

std::size_t RayList::size() {
    return m_rayList.size();
}

int RayList::rayAmount() const {
    //std::cout<<"raylist back size= "<< (m_rayList.back()).size() <<std::endl; 
    //std::cout<<"raylist size= "<< m_rayList.size() <<std::endl; 
    int amount = (m_rayList.size() - 1) * RAY_MAX_ELEMENTS_IN_VECTOR + (m_rayList.back()).size();
    return amount;
}