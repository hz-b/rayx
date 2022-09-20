#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::writeBuffer(const char* bufname, char* indata) {
    Buffer& b = m_buffers[bufname];

    if (!b.m_in) {
        RAYX_ERR << "writeBuffer(\"" << bufname
                 << "\", ...) is not allowed, as \"" << bufname
                 << "\" has m_in = false";
    }

    size_t remainingBytes = m_buffers[bufname].m_size;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min(remainingBytes, (size_t)STAGING_SIZE);
        storeToStagingBuffer(indata + offset, localbytes);
        gpuMemcpy(m_stagingBuffer, 0, m_buffers[bufname].m_Buffer, offset,
                  localbytes);

        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

}  // namespace RAYX
