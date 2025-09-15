#pragma once

#include <vector>

#include "Core.h"

namespace RAYX {

class RAYX_API ObjectRecordMask {
  public:
    static ObjectRecordMask none(const int numSources, const int numElements) {
        return {
            .sourceMask  = std::vector<bool>(numSources, false),
            .elementMask = std::vector<bool>(numElements, false),
        };
    }

    // TODO: would ne nice if this did not require to know numSources and numElements, because then, ObjectRecordMask in the paramter list of function
    // Tracer::trace, could take a default value
    static ObjectRecordMask all(const int numSources, const int numElements) {
        return {
            .sourceMask  = std::vector<bool>(numSources, true),
            .elementMask = std::vector<bool>(numElements, true),
        };
    }

    static ObjectRecordMask allSources(const int numSources, const int numElements) {
        return {
            .sourceMask  = std::vector<bool>(numSources, true),
            .elementMask = std::vector<bool>(numElements, false),
        };
    }

    static ObjectRecordMask allElements(const int numSources, const int numElements) {
        return {
            .sourceMask  = std::vector<bool>(numSources, false),
            .elementMask = std::vector<bool>(numElements, true),
        };
    }

    static ObjectRecordMask fromSourceIndices(const int numSources, const int numElements, const std::vector<int>& sourceIndices) {
        auto sourceMask = std::vector<bool>(numSources, false);
        for (const auto i : sourceIndices) sourceMask.at(i) = true;
        return {
            .sourceMask  = sourceMask,
            .elementMask = std::vector<bool>(numElements, false),
        };
    }

    static ObjectRecordMask fromElementIndices(const int numSources, const int numElements, const std::vector<int>& elementIndices) {
        auto elementMask = std::vector<bool>(numElements, false);
        for (const auto i : elementIndices) elementMask.at(i) = true;
        return {
            .sourceMask  = std::vector<bool>(numElements, false),
            .elementMask = elementMask,
        };
    }

    static ObjectRecordMask fromObjectIndices(const int numSources, const int numElements, const std::vector<int>& objectIndices) {
        auto sourceMask  = std::vector<bool>(numSources, false);
        auto elementMask = std::vector<bool>(numElements, false);

        for (const auto i : objectIndices) {
            if (i < numSources)
                sourceMask.at(i) = true;
            else
                elementMask.at(i - numSources) = true;
        }

        return {
            .sourceMask  = sourceMask,
            .elementMask = elementMask,
        };
    }

    bool shouldRecordObject(int object_id) const {
        const auto numSources = static_cast<int>(sourceMask.size());
        if (object_id < numSources) return sourceMask.at(object_id);
        return elementMask.at(object_id - numSources);
    }

    void setShouldRecordObject(int object_id, bool value) {
        const auto numSources = static_cast<int>(sourceMask.size());
        if (object_id < numSources) sourceMask.at(object_id) = value;
        elementMask.at(object_id - numSources) = value;
    }

    int numSources() const { return static_cast<int>(sourceMask.size()); }
    int numElements() const { return static_cast<int>(elementMask.size()); }
    int numObjects() const { return numSources() + numElements(); }

    int numSourcesToRecord() const { return static_cast<int>(std::count(sourceMask.begin(), sourceMask.end(), true)); }
    int numElementsToRecord() const { return static_cast<int>(std::count(elementMask.begin(), elementMask.end(), true)); }
    int numObjectsToRecord() const { return numSourcesToRecord() + numElementsToRecord(); }

    bool shouldRecordSource(int source_id) const { return sourceMask.at(source_id); }
    void setShouldRecordSource(int source_id, bool value) { sourceMask.at(source_id) = value; }

    bool shouldRecordElement(int element_id) const { return elementMask.at(element_id); }
    void setShouldRecordElement(int element_id, bool value) { elementMask.at(element_id) = value; }

  private:
    std::vector<bool> sourceMask;
    std::vector<bool> elementMask;
};

}  // namespace RAYX
