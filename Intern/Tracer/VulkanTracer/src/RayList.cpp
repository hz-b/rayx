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
    std::cout << "input size= " << inputSize << std::endl;
    if (m_rayList.size() == 0) {
        lastVectorSize = 0;
    }
    else {
        lastVectorSize = (m_rayList.back()).size();
    }
    std::cout << "last vector size= " << lastVectorSize << std::endl;
    if (lastVectorSize == RAY_MAX_ELEMENTS_IN_VECTOR || lastVectorSize == 0) {
        //if input is smaller than the max allowed size, we can resize and append
        if (inputSize <= RAY_MAX_ELEMENTS_IN_VECTOR) {
            m_rayList.push_back(input);
            m_rayList.back().resize(inputSize);
            memcpy(&(m_rayList.back()[0]), location, inputSize * RAY_DOUBLE_COUNT * sizeof(double));
        }
        //if input is larger, we need to split it
        else {
            size_t i = 0;
            for (; i < (inputSize - RAY_MAX_ELEMENTS_IN_VECTOR); i = i + RAY_MAX_ELEMENTS_IN_VECTOR) {
                //std::cout << "size= " << m_rayList.size() << std::endl;
                addVector();
                //std::cout << "size= " << m_rayList.size() << std::endl;
                //std::cout<<"capacity= "<< m_rayList.front().capacity() <<std::endl;
                m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
                memcpy(&(m_rayList.back()[0]), location, RAY_VECTOR_SIZE);
                m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
            }
            addVector();
            size_t remainingBytes = (inputSize * RAY_DOUBLE_COUNT * sizeof(double)) - (i * RAY_DOUBLE_COUNT * sizeof(double));
            //std::cout << "insertVector: remainingBytes= " << remainingBytes << std::endl;
            m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
            memcpy((m_rayList.back().data()), (char*)location + (i * RAY_DOUBLE_COUNT * sizeof(double)), remainingBytes);
            // std::cout << "ray 16384 xpos: " << (m_rayList.back())[16384].getxPos() << std::endl;
            // std::cout << "ray 16383 xpos: " << (m_rayList.back())[16383].getxPos() << std::endl;
            // std::cout << "ray 16385 xpos: " << (m_rayList.back())[16385].getxPos() << std::endl;
            // std::cout << "ray 16386 xpos: " << (m_rayList.back())[16386].getxPos() << std::endl;
            //std::cout << "insertVector: memcpy done" << std::endl;
            m_rayList.back().resize(remainingBytes / (RAY_DOUBLE_COUNT * sizeof(double)));
        }
    }
    else if (lastVectorSize < RAY_MAX_ELEMENTS_IN_VECTOR) {

        //copy input to end of the last vector
        auto bytesNeededToFillLastVector = std::min(inputSize * RAY_DOUBLE_COUNT * sizeof(double), RAY_VECTOR_SIZE - (lastVectorSize * RAY_DOUBLE_COUNT * sizeof(double)));
        std::cout << "bytesNeededToFillLastVector= " << bytesNeededToFillLastVector << std::endl;
        std::cout << "capacity= " << (m_rayList.back()).capacity() << std::endl;
        std::cout << "inputSize= " << inputSize << std::endl;
        (m_rayList.back()).reserve((m_rayList.back()).size() + (bytesNeededToFillLastVector / (RAY_DOUBLE_COUNT * sizeof(double))));
        std::cout << "capacity= " << (m_rayList.back()).capacity() << std::endl;
        memcpy(&((m_rayList.back()).back()) + 1, location, bytesNeededToFillLastVector);
        m_rayList.back().resize(m_rayList.back().size() + (bytesNeededToFillLastVector / (RAY_DOUBLE_COUNT * sizeof(double))));
        std::cout << "size after resize= " << (m_rayList.back()).size() << std::endl;
        int i = bytesNeededToFillLastVector;
        for (;i < (int(inputSize) * RAY_DOUBLE_COUNT * sizeof(double)) - RAY_VECTOR_SIZE; i = i + RAY_VECTOR_SIZE) {
            addVector();

            std::cout << "insert vector: reserved" << std::endl;
            memcpy(&((m_rayList.back())[0]), (char*)location + (i), RAY_VECTOR_SIZE);
            m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
        }
        int remainingElements = int(inputSize) - (i / (RAY_DOUBLE_COUNT * sizeof(double)));
        std::cout << "remaining elements: " << remainingElements << std::endl;
        if (remainingElements > 0) {
            std::cout << remainingElements << std::endl;
            std::cout << "test4" << std::endl;
            addVector();
            memcpy(&((m_rayList.back())[0]), location, remainingElements);
            m_rayList.back().resize(remainingElements);
        }
    }
    //this else should never be entered
    else {
        throw std::runtime_error("last vector larger than RAY_VECTOR_SIZE");
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