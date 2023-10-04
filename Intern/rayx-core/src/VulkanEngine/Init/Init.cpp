#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::init(VulkanEngineInitSpec spec) {
    if (m_state != VulkanEngineStates::PREINIT) {
        RAYX_ERR << "VulkanEngine was already initialized!";
    }

    m_shaderFile = spec.shaderFileName;
    m_deviceID = spec.deviceID;

    createInstance();
    setupDebugMessenger();
    pickDevice();
    createCommandPool();
    createCommandBuffers();
    createSemaphores();
    createDescriptorSetLayout();
    createAllocateDescriptorPool(m_buffers.size());
    createFences();
    prepareVma();
    createStagingBuffer();
    createCache();
    createShaderModule();
    m_state = VulkanEngineStates::PRERUN;
}

}  // namespace RAYX

#endif
