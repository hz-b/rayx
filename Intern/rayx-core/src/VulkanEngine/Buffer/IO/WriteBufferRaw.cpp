#ifndef NO_VULKAN

#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::writeBufferRaw(const char* buffName, char* inData) {
    Buffer& b = m_buffers[buffName];

    if (!b.isInput) {
        RAYX_ERR << "writeBufferRaw(\"" << buffName << "\", ...) is not allowed, as \"" << buffName << "\" has m_in = false";
    }

    size_t remainingBytes = m_buffers[buffName].size;
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localBytes = std::min(remainingBytes, (size_t)STAGING_SIZE);
        storeToStagingBuffer(inData + offset, localBytes);
        gpuMemcpy(m_buffers[buffName].buf, offset, m_stagingBuffer.buf, 0, localBytes);

        offset += localBytes;
        remainingBytes -= localBytes;
        m_Fences.transfer->wait();
    }
}

void VulkanEngine::storeToStagingBuffer(char* inData, size_t bytes) {
    // void* buf;
    // vkMapMemory(m_Device, m_stagingMemory, 0, STAGING_SIZE, 0, &buf);

    memcpy(m_stagingBuffer.allocaInfo.pMappedData, inData, bytes);

    // vkUnmapMemory(m_Device, m_stagingMemory);
}

}  // namespace RAYX

#endif
