#include "RayList.h"
#include <cassert>
#include <cmath>
#include <cstring>

RayList::RayList() {
    //addVector();
}

RayList::~RayList() {}

void RayList::addVector() {
    std::cout << "add vector: start" << std::endl;
    if (m_rayList.size() > 0) {

        //std::cout<<"addVector: last vector size= "<< m_rayList.back().size() <<std::endl;    
        assert(("Size of last Ray vector is not equal to RAY_VECTOR_SIZE", (m_rayList.back()).size() == RAY_MAX_ELEMENTS_IN_VECTOR));

    }
    std::vector<Ray> newRayVector;
    //std::cout<<"add vector: capacity: "<<newRayVector.capacity()<<std::endl;
    m_rayList.push_back(newRayVector);
    //std::cout<<"add vector: reserving"<<std::endl;
    m_rayList.back().reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
    std::cout << "add vector: end" << std::endl;
}

void RayList::insertVector(std::vector<Ray> input) {
    //std::cout<<"insert vector: start"<<std::endl;
    size_t lastVectorSize = (m_rayList.back()).size();
    auto inputSize = input.size();
    //if the last vector of the list is full, we can just append
    //std::cout<<"RayList size= "<< m_rayList.size() <<std::endl;
    //std::cout<<"input size= "<< inputSize <<std::endl;
    if (m_rayList.size() == 0) {
        lastVectorSize = 0;
    }
    std::cout << "last vector size= " << lastVectorSize << std::endl;
    if (lastVectorSize == RAY_MAX_ELEMENTS_IN_VECTOR || lastVectorSize == 0) {
        //if input is smaller than the max allowed size, we can resize and append
        if (inputSize <= RAY_MAX_ELEMENTS_IN_VECTOR) {
            input.reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
            m_rayList.push_back(input);
        }
        //if input is larger, we need to split it
        else {
            int i = 0;
            for (; i < inputSize - RAY_MAX_ELEMENTS_IN_VECTOR; i = i + RAY_MAX_ELEMENTS_IN_VECTOR) {
                std::cout << "size= " << m_rayList.size() << std::endl;
                addVector();
                //std::cout<<"size= "<< m_rayList.size() <<std::endl;        
                //std::cout<<"capacity= "<< m_rayList.front().capacity() <<std::endl;        
                memcpy(&(m_rayList.back().front()), &(input[i]), RAY_VECTOR_SIZE);
                m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
            }
            std::cout << "test2" << std::endl;
            addVector();
            size_t remainingBytes = (inputSize * RAY_DOUBLE_COUNT * sizeof(double)) - (i * RAY_DOUBLE_COUNT * sizeof(double));
            //std::cout<<"insertVector: remainingBytes= "<<remainingBytes<<std::endl;
            memcpy(&((m_rayList.back())[0]), &(input[i]), remainingBytes);
            //std::cout<<"insertVector: memcpy done"<<std::endl;
            m_rayList.back().resize(remainingBytes / (RAY_DOUBLE_COUNT * sizeof(double)));
        }
    }
    else if (lastVectorSize < RAY_MAX_ELEMENTS_IN_VECTOR) {

        //copy input to end of the last vector
        auto bytesNeededToFillLastVector = std::min(inputSize * RAY_DOUBLE_COUNT * sizeof(double), RAY_VECTOR_SIZE - (lastVectorSize * RAY_DOUBLE_COUNT * sizeof(double)));
        std::cout << "bytesNeededToFillLastVector= " << bytesNeededToFillLastVector << std::endl;
        std::cout << "capacity= " << (m_rayList.back()).capacity() << std::endl;
        std::cout << "size= " << (m_rayList.back()).size() << std::endl;
        std::cout << "inputsize= " << inputSize << std::endl;
        (m_rayList.back()).reserve((m_rayList.back()).size() + (bytesNeededToFillLastVector / (RAY_DOUBLE_COUNT * sizeof(double))));
        std::cout << "capacity= " << (m_rayList.back()).capacity() << std::endl;
        memcpy(&((m_rayList.back()).back()) + 1, &(input[0]), bytesNeededToFillLastVector);
        m_rayList.back().resize(m_rayList.back().size() + (bytesNeededToFillLastVector / (RAY_DOUBLE_COUNT * sizeof(double))));
        std::cout << "test" << std::endl;
        int i = bytesNeededToFillLastVector;
        for (;i < (int(inputSize) * RAY_DOUBLE_COUNT) - RAY_VECTOR_SIZE; i = i + RAY_VECTOR_SIZE) {
            std::cout << "test3" << std::endl;
            addVector();

            std::cout << "insert vector: reserved" << std::endl;
            memcpy(&((m_rayList.back())[0]), &(input[i]), RAY_VECTOR_SIZE);
            m_rayList.back().resize(RAY_MAX_ELEMENTS_IN_VECTOR);
        }
        int remainingElements = int(inputSize) - (i);
        if (remainingElements > 0) {
            std::cout << remainingElements << std::endl;
            std::cout << "test4" << std::endl;
            addVector();
            memcpy(&((m_rayList.back())[0]), &(input[i]), remainingElements);
            m_rayList.back().resize(remainingElements);
        }
    }
    //this else should never be entered
    else {
        throw std::runtime_error("last vector larger than RAY_VECTOR_SIZE");
    }

};
std::list<std::vector<Ray>>::iterator RayList::begin() {
    return m_rayList.begin();
};
std::list<std::vector<Ray>>::iterator RayList::end() {
    return m_rayList.end();
};
std::size_t RayList::size() {
    return m_rayList.size();
};
int RayList::rayAmount() {
    //std::cout<<"raylist back size= "<< (m_rayList.back()).size() <<std::endl; 
    //std::cout<<"raylist size= "<< m_rayList.size() <<std::endl; 
    int amount = (m_rayList.size() - 1) * RAY_MAX_ELEMENTS_IN_VECTOR + (m_rayList.back()).size();
    return amount;
}