#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::init(InitSpec spec) {
    if (m_state != EngineState::PREINIT) {
        RAYX_ERR << "VulkanEngine was already initialized!";
    }

    m_shaderfile = spec.m_shader;

    createInstance();
    setupDebugMessenger();
    pickDevice();
    createCommandPool();
    createDescriptorSetLayout();
    createStagingBuffer();

    m_state = EngineState::PRERUN;
}

}  // namespace RAYX
