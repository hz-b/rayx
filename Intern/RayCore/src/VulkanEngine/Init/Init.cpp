#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::init(VulkanEngineInitSpec_t spec) {
    if (m_state != EngineStates_t::PREINIT) {
        RAYX_ERR << "VulkanEngine was already initialized!";
    }

    m_shaderfile = spec.shaderFileName;

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
    m_state = EngineStates_t::PRERUN;
}

/**
 * @brief This Function initilizes primordial Vulkan things (Instance, Device, etc.)
 *
 */
void VulkanEngine::newInit() {
    createInstance();
    setupDebugMessenger();
    pickDevice();
    createCommandPool();      // FIXME(OS) So far we need one GlobalPool and
    createCommandBuffers(1);  // 1 Command Buffer
    newCreateSemaphores(1);   // FIXME(OS) We only need one Sempahore (+1 for Transfer in handler)
    prepareVma();
    createCache();
    initBufferHandler();
    m_state = EngineStates_t::PRERUN;
}

/**
 * @brief Initializes the Buffer Handler
 * Call after 'init'
 *
 */
void VulkanEngine::initBufferHandler() {
    m_BufferHandler = std::make_unique<BufferHandler>(m_Device, m_VmaAllocator, m_computeFamily, getStagingBufferSize());
}

void VulkanEngine::createComputePipelinePass(const ComputePassCreateInfo& createInfo) {
    m_ComputePass = std::make_unique<ComputePass>(m_Device, createInfo);
}

// Buffer Descriptor binding
void VulkanEngine::prepareComputePipelinePass() {
    auto handler = getBufferHandler();
    auto bindings = handler->getDescriptorBindings(m_ComputePass->getName());
    m_ComputePass->prepare(bindings);
}

}  // namespace RAYX

#endif
