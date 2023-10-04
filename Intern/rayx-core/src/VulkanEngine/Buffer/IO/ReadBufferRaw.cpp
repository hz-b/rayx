#ifndef NO_VULKAN

#include <algorithm>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::readBufferRaw(const char* buffName, char* outData) {
    if (m_state != VulkanEngineStates::POSTRUN) {
        RAYX_ERR << "you've forgotten to .run() the VulkanEngine. That's "
                    "mandatory before reading it's output buffers.";
    }

    Buffer& b = m_buffers[buffName];

    if (!b.isOutput) {
        RAYX_ERR << "readBufferRaw(\"" << buffName << "\", ...) is not allowed, as \"" << buffName << "\" has m_out = false";
    }

    size_t remainingBytes = b.size;
    size_t offset = 0;
    vkQueueWaitIdle(m_ComputeQueue);
    while (remainingBytes > 0) {
        size_t localBytes = std::min((size_t)STAGING_SIZE, remainingBytes);
        gpuMemcpy(m_stagingBuffer.buf, 0, b.buf, offset, localBytes);
        m_Fences.transfer->wait();
        loadFromStagingBuffer(outData + offset, localBytes);
        offset += localBytes;
        remainingBytes -= localBytes;
    }
}

void VulkanEngine::loadFromStagingBuffer(char* outData, size_t bytes) {
    // void* buf;
    // vkMapMemory(m_Device, m_stagingMemory, 0, bytes, 0, &buf);

    memcpy(outData, m_stagingBuffer.allocaInfo.pMappedData, bytes);

    // vkUnmapMemory(m_Device, m_stagingMemory);
}

}  // namespace RAYX

#endif
