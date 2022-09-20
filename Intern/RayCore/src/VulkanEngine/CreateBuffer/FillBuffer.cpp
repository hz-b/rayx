#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::fillBuffer(const char* bufname, char* data, uint32_t bytes) {
    size_t remainingBytes = bytes;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min(remainingBytes, (size_t)STAGING_SIZE);
        storeToStagingBuffer(data + offset, localbytes);
        gpuMemcpy(m_stagingBuffer, 0, m_buffers[bufname].m_Buffer, offset,
                  localbytes);

        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

}  // namespace RAYX
