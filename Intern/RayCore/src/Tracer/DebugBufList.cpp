#include "DebugBufList.h"

#include <cmath>
#include <cstring>
#include <iostream>

namespace RAYX {
template <typename T>
DebugBufList<T>::DebugBufList() {}

template <typename T>
DebugBufList<T>::~DebugBufList() {}

template <typename T>
void DebugBufList<T>::insertVector(std::vector<T>&& inDebugVector) {
    // check if last vector is full, if not fill it
    size_t remainingSpace = 0;
    if ((m_List.back().size() > 0) &&
        (m_List.back().size() < DEBUG_BUF_DOUBLE_COUNT)) {
        remainingSpace = DEBUG_BUF_DOUBLE_COUNT - m_List.back().size();
        if (remainingSpace > 0) {
            // fill remaining space
            m_List.back().resize(DEBUG_BUF_DOUBLE_COUNT);
            m_List.back().insert(m_List.back().end(), inDebugVector.begin(),
                                 inDebugVector.begin() + remainingSpace);
            // erase from vector
            inDebugVector.erase(inDebugVector.begin(),
                                inDebugVector.begin() + remainingSpace);
        }
    }

    // cut inDebugVector to RAY_VECTOR_SIZE big pieces and emplace back
    int numberOfVecs = inDebugVector.size() / DEBUG_BUF_DOUBLE_COUNT;
    if (inDebugVector.size() % DEBUG_BUF_DOUBLE_COUNT != 0) {
        numberOfVecs++;
    }
    for (int i = 0; i < numberOfVecs; i++) {
        if (i == numberOfVecs - 1) {
            // last vector
            m_List.emplace_back(
                inDebugVector.begin() + i * DEBUG_BUF_DOUBLE_COUNT,
                inDebugVector.end());
        } else {
            m_List.emplace_back(
                inDebugVector.begin() + i * DEBUG_BUF_DOUBLE_COUNT,
                inDebugVector.begin() + (i + 1) * DEBUG_BUF_DOUBLE_COUNT);
        }
    }
}

template <typename T>
void DebugBufList<T>::clean() {
    m_List.clear();
}

template <typename T>
std::list<std::vector<T>>::iterator DebugBufList<T>::begin() {
    return m_List.begin();
}

template <typename T>
std::list<std::vector<T>>::iterator DebugBufList<T>::end() {
    return m_List.end();
}

template <typename T>
std::vector<T> DebugBufList<T>::back() {
    return m_List.back;
}

template <typename T>
std::size_t DebugBufList<T>::size() {
    return m_List.size();
}

template <typename T>
int DebugBufList<T>::getAmount() const {
    int amount =
        (m_List.size() - 1) * DEBUG_BUF_DOUBLE_COUNT + (m_List.back()).size();
    return amount;
}

}  // namespace RAYX