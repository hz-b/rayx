#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::readOutBufferRaw(const char* bufname, char* outdata,
                                    uint32_t bytes) {
    if (m_state != EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
                    "mandatory before reading it's output buffers.";
    }

    Buffer& b = m_buffers[bufname];

    size_t remainingBytes = bytes;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min((size_t)STAGING_SIZE, remainingBytes);
        gpuMemcpy(b.m_Buffer, offset, m_stagingBuffer, 0, localbytes);
        loadFromStagingBuffer(outdata + offset, localbytes);
        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

}  // namespace RAYX
