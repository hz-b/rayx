#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::readBuffer(const char* bufname, char* outdata) {
    if (m_state != EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
                    "mandatory before reading it's output buffers.";
    }

    Buffer& b = m_buffers[bufname];

    if (!b.m_out) {
        RAYX_ERR << "readBuffer(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" has m_out = false";
    }

    size_t remainingBytes = b.m_size;
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
