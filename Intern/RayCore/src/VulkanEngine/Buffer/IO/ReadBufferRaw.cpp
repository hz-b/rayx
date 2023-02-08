#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::readBufferRaw(const char* bufname, char* outdata) {
    if (m_state != VulkanEngineStates_t::POSTRUN) {
        RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
                    "mandatory before reading it's output buffers.";
    }

    Buffer_t& b = m_buffers[bufname];

    if (!b.isOutput) {
        RAYX_ERR << "readBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" has m_out = false";
    }

    size_t remainingBytes = b.size;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min((size_t)STAGING_SIZE, remainingBytes);
        gpuMemcpy(m_stagingBuffer.buf, 0, b.buf, offset, localbytes);
        m_Fences.transfer->wait();
        loadFromStagingBuffer(outdata + offset, localbytes);
        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

void VulkanEngine::loadFromStagingBuffer(char* outdata, size_t bytes) {
    // void* buf;

    // vkMapMemory(m_Device, m_stagingMemory, 0, bytes, 0, &buf);

    memcpy(outdata, m_stagingBuffer.allocaInfo.pMappedData, bytes);

    // vkUnmapMemory(m_Device, m_stagingMemory);
}

}  // namespace RAYX
