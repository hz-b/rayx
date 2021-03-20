#include "RayList.h"
#include <cassert>
#include <cmath>
#include <cstring>

RayList::RayList(){
    //addVector();
}
    
RayList::~RayList(){}

void RayList::addVector(){
    if(m_rayList.size() >0){
        assert(("Size of last Ray vector is not equal to RAY_VECTOR_SIZE", ((m_rayList.back()).size()*RAY_DOUBLE_COUNT) == RAY_VECTOR_SIZE));

    }
    std::vector<Ray> newRayVector;
    newRayVector.reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
    std::cout<<"capacity: "<<newRayVector.capacity()<<std::endl;
    m_rayList.push_back(newRayVector);
}

void RayList::insertVector(std::vector<Ray> input){
    size_t lastVectorSize = (m_rayList.back()).size();
    auto inputSize = input.size();
    //if the last vector of the list is full, we can just append
    if(lastVectorSize == RAY_MAX_ELEMENTS_IN_VECTOR || lastVectorSize == 0){
        //if input is smaller than the max allowed size, we can resize and append
        if(inputSize <= RAY_MAX_ELEMENTS_IN_VECTOR){
            input.reserve(RAY_MAX_ELEMENTS_IN_VECTOR);
            m_rayList.push_back(input);
        }
        //if input is larger, we need to split it
        else{
            int i = 0;
            for(; i<(inputSize*RAY_DOUBLE_COUNT*sizeof(double))-RAY_VECTOR_SIZE; i = i+RAY_VECTOR_SIZE){
                addVector();
                memcpy(&((m_rayList.back())[0]), &(input[i]), RAY_VECTOR_SIZE);
            }
            addVector();
            auto remainingElements = inputSize - (i);
            memcpy(&((m_rayList.back())[0]), &(input[i]), remainingElements);
        }
    }
    else if(lastVectorSize < RAY_MAX_ELEMENTS_IN_VECTOR){
        
        //copy input to end of the last vector
        auto bytesNeededToFillLastVector = std::min(inputSize*RAY_DOUBLE_COUNT*sizeof(double), RAY_VECTOR_SIZE - (lastVectorSize*RAY_DOUBLE_COUNT*sizeof(double)));   
        std::cout<<"size= "<< m_rayList.size() <<std::endl;               
        std::cout<<"capacity= "<< (m_rayList.back()).capacity() <<std::endl;               
        memcpy(&((m_rayList.back()).back()), &(input[0]), bytesNeededToFillLastVector);
        int i = bytesNeededToFillLastVector;
        for(;i < (inputSize*RAY_DOUBLE_COUNT)-RAY_VECTOR_SIZE; i = i+RAY_VECTOR_SIZE){
            addVector();
            memcpy(&((m_rayList.back())[0]), &(input[i]), RAY_VECTOR_SIZE);
        }
        addVector();
            auto remainingElements = inputSize - (i);
            memcpy(&((m_rayList.back())[0]), &(input[i]), remainingElements);
    }
    //this else should never be entered
    else{
        throw std::runtime_error("last vector larger than RAY_VECTOR_SIZE");
    }

};
std::list<std::vector<Ray>>::iterator RayList::begin(){
    return m_rayList.begin();
};
std::list<std::vector<Ray>>::iterator RayList::end(){
    return m_rayList.end();
};
std::size_t RayList::size(){
    return m_rayList.size();
};
int RayList::rayAmount(){
    std::cout<<"raylist back size= "<< (m_rayList.back()).size() <<std::endl; 
    std::cout<<"raylist size= "<< m_rayList.size() <<std::endl; 
    int amount = (m_rayList.size()-1)*RAY_MAX_ELEMENTS_IN_VECTOR + (m_rayList.back()).size();
    return amount;
}