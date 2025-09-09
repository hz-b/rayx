#pragma once

namespace RAYX {

struct RecordMask {
    int numSources;
    bool* __restrict mask;

    RAYX_FN_ACC inline bool shouldRecordSource(const int sourceIndex) const { return mask[sourceIndex]; }
    RAYX_FN_ACC inline bool shouldRecordElement(const int elementIndex) const { return mask[elementIndex + numSources]; }
};

}  // namespace RAYX
