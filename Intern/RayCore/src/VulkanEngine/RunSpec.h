#pragma once

namespace RAYX {

class RunSpec {
  public:
    RunSpec() = default;

    inline RunSpec numberOfInvocations(uint32_t arg) {
        m_numberOfInvocations = arg;
        return *this;
    }

    inline RunSpec computeBuffersCount(uint32_t arg) {
        m_computeBuffersCount = arg;
        return *this;
    }

    inline RunSpec buffer_with_data(const char* buffername, GpuData data) {
        buffersizes[buffername] = data.size();
        bufferdata[buffername] = data;
        return *this;
    }

    inline RunSpec buffer_with_size(const char* buffername, uint32_t size) {
        buffersizes[buffername] = size;
        return *this;
    }

    uint32_t m_numberOfInvocations;
    uint32_t m_computeBuffersCount;
    dict<GpuData>
        bufferdata;  // contains the data of only the initialized buffers
    dict<uint32_t> buffersizes;  // contains the size for *all* buffers.
};

}  // namespace RAYX
