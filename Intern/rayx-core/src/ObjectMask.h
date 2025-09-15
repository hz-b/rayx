#pragma once

#include <vector>

#include "Core.h"

namespace RAYX {

class RAYX_API ObjectMask {
    ObjectMask(const std::vector<bool>& sourceMask, const std::vector<bool>& elementMask) : m_sourceMask(sourceMask), m_elementMask(elementMask) {}
    ObjectMask(std::vector<bool>&& sourceMask, std::vector<bool>&& elementMask) : m_sourceMask(std::move(sourceMask)), m_elementMask(std::move(elementMask)) {}

  public:
    ObjectMask() = default;
    ObjectMask(const ObjectMask&) = default;
    ObjectMask(ObjectMask&&) = default;
    ObjectMask& operator=(const ObjectMask&) = default;
    ObjectMask& operator=(ObjectMask&&) = default;

    static ObjectMask none(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, false),
            std::vector<bool>(numElements, false),
        };
    }

    // TODO: would ne nice if this did not require to know numSources and numElements, because then, ObjectMask in the paramter list of function
    // Tracer::trace, could take a default value
    static ObjectMask all(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, true),
            std::vector<bool>(numElements, true),
        };
    }

    static ObjectMask allSources(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, true),
            std::vector<bool>(numElements, false),
        };
    }

    static ObjectMask allElements(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, false),
            std::vector<bool>(numElements, true),
        };
    }

    static ObjectMask fromSourceIndices(const int numSources, const int numElements, const std::vector<int>& sourceIndices) {
        auto sourceMask = std::vector<bool>(numSources, false);
        for (const auto i : sourceIndices) sourceMask.at(i) = true;
        return {
            sourceMask,
            std::vector<bool>(numElements, false),
        };
    }

    static ObjectMask fromElementIndices(const int numSources, const int numElements, const std::vector<int>& elementIndices) {
        auto elementMask = std::vector<bool>(numElements, false);
        for (const auto i : elementIndices) elementMask.at(i) = true;
        return {
            std::vector<bool>(numSources, false),
            elementMask,
        };
    }

    static ObjectMask fromObjectIndices(const int numSources, const int numElements, const std::vector<int>& objectIndices) {
        auto sourceMask  = std::vector<bool>(numSources, false);
        auto elementMask = std::vector<bool>(numElements, false);

        for (const auto i : objectIndices) {
            if (i < numSources)
                sourceMask.at(i) = true;
            else
                elementMask.at(i - numSources) = true;
        }

        return {
            sourceMask,
            elementMask,
        };
    }

    int numSources() const { return static_cast<int>(m_sourceMask.size()); }
    int numElements() const { return static_cast<int>(m_elementMask.size()); }
    int numObjects() const { return numSources() + numElements(); }

    int numSourcesToRecord() const { return static_cast<int>(std::count(m_sourceMask.begin(), m_sourceMask.end(), true)); }
    int numElementsToRecord() const { return static_cast<int>(std::count(m_elementMask.begin(), m_elementMask.end(), true)); }
    int numObjectsToRecord() const { return numSourcesToRecord() + numElementsToRecord(); }

    bool shouldRecordSource(int source_id) const { return m_sourceMask.at(source_id); }
    void setShouldRecordSource(int source_id, bool value) { m_sourceMask.at(source_id) = value; }

    bool shouldRecordElement(int element_id) const { return m_elementMask.at(element_id); }
    void setShouldRecordElement(int element_id, bool value) { m_elementMask.at(element_id) = value; }

    bool shouldRecordObject(int object_id) const {
        const auto numSources = static_cast<int>(m_sourceMask.size());
        if (object_id < numSources) return m_sourceMask.at(object_id);
        return m_elementMask.at(object_id - numSources);
    }

    void setShouldRecordObject(int object_id, bool value) {
        const auto numSources = static_cast<int>(m_sourceMask.size());
        if (object_id < numSources) m_sourceMask.at(object_id) = value;
        m_elementMask.at(object_id - numSources) = value;
    }

  private:
    std::vector<bool> m_sourceMask;
    std::vector<bool> m_elementMask;
};

}  // namespace RAYX
