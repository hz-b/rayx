#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::createBufferWithDataRaw(const char* bufname, GpuData data) {
    createBuffer(bufname, data.size());
    fillBuffer(bufname, data);
}

void VulkanEngine::createBuffer(const char* bufname, VkDeviceSize size) {
    if (m_state == EngineState::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }

    Buffer& b = m_buffers[bufname];
    b.m_size = size;

    int buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (b.m_in) {
        buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (b.m_out) {
        buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    createVkBuffer(size, buffer_usage_flags,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, b.m_Buffer, b.m_Memory);

}

}  // namespace RAYX
