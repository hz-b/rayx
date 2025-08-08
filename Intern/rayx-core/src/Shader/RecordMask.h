#pragma once

#include <vector>

#include "Debug/Debug.h"

namespace RAYX {

RAYX_API inline std::vector<bool> recordMaskNone(const int numObjects) { return std::vector<bool>(numObjects, false); }

RAYX_API inline std::vector<bool> recordMaskAll(const int numObjects) { return std::vector<bool>(numObjects, true); }

RAYX_API inline std::vector<bool> recordMaskAllSources(const int numSources, const int numElements) {
    auto mask = std::vector<bool>(numSources + numElements, false);
    for (int i = 0; i < numSources; ++i) mask[i] = true;
    return mask;
}

RAYX_API inline std::vector<bool> recordMaskAllElements(const int numSources, const int numElements) {
    auto mask = std::vector<bool>(numSources + numElements, false);
    for (int i = 0; i < numElements; ++i) mask[i + numSources] = true;
    return mask;
}

RAYX_API inline std::vector<bool> recordMaskByIndices(const int numObjects, const std::vector<int>& indices) {
    auto mask = std::vector<bool>(numObjects, false);

    for (const auto i : indices) {
        if (i < 0) RAYX_EXIT << "error: index '" << i << "' provided for record indices, but only positive indices are allowed";
        if (i >= numObjects) RAYX_EXIT << "error: index '" << i << "' provided for record indices, but this index is out of range.";
        mask[i] = true;
    }

    return mask;
}

struct RecordMask {
    int numSources;
    bool* __restrict mask;

    RAYX_FN_ACC inline bool shouldRecordSource(const int sourceIndex) const { return mask[sourceIndex]; }
    RAYX_FN_ACC inline bool shouldRecordElement(const int elementIndex) const { return mask[elementIndex + numSources]; }
};

}  // namespace RAYX
