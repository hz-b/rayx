#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::defineBufferByDataRaw(const char* bufname, GpuData data) {
    if (m_state == EngineState::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }

    createBuffer(bufname, data.size());
    fillBuffer(bufname, data);
}

void VulkanEngine::defineBufferBySize(const char* bufname, VkDeviceSize size) {
    if (m_state == EngineState::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }

    createBuffer(bufname, size);
}

}  // namespace RAYX
