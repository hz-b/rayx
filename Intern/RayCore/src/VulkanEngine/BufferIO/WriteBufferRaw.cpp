#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::writeBufferRaw(const char* bufname, char* indata) {
    Buffer& b = m_buffers[bufname];

    if (!b.m_in) {
        RAYX_ERR << "writeBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" has m_in = false";
    }

    size_t remainingBytes = m_buffers[bufname].m_size;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min(remainingBytes, (size_t)STAGING_SIZE);
        storeToStagingBuffer(indata + offset, localbytes);
        gpuMemcpy(m_stagingBuffer.m_Buffer, 0, m_buffers[bufname].m_Buffer, offset, localbytes);

        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

void VulkanEngine::storeToStagingBuffer(char* indata, size_t bytes) {
    // void* buf;
    // vkMapMemory(m_Device, m_stagingMemory, 0, STAGING_SIZE, 0, &buf);

    memcpy(m_stagingBuffer.m_BufferAllocationInfo.pMappedData, indata, bytes);

    // vkUnmapMemory(m_Device, m_stagingMemory);
}

}  // namespace RAYX
