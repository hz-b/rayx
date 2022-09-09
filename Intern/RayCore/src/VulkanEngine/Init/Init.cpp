#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::init(InitSpec spec) {
    m_shaderfile = spec.m_shader;

    createInstance();
    setupDebugMessenger();
    pickDevice();
    createCommandPool();
    createDescriptorSetLayout();
    createStagingBuffer();
}

}  // namespace RAYX
