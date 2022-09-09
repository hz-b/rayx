#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

GpuData VulkanEngine::readOutBufferRaw(const char* bufname) {
    if (m_state != EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
                    "mandatory before reading it's output buffers.";
    }


    GpuData out;
    Buffer& b = m_buffers[bufname];

    uint32_t remaining_bytes = b.m_size;
    int offset = 0;
    while (remaining_bytes > 0) {
        int localbytes = std::min(STAGING_SIZE, remaining_bytes);
        gpuMemcpy(b.m_Buffer, offset, m_stagingBuffer, 0, localbytes);
        out.raw.push_back(loadFromStagingBuffer(localbytes));
        offset += localbytes;
        remaining_bytes -= localbytes;
    }

    return out;
}

}  // namespace RAYX
