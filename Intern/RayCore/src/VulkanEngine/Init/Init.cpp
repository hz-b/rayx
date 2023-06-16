#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief This Function initilizes primordial Vulkan things (Instance, Device, etc.)
 *
 */
void VulkanEngine::init() {
    createInstance();
    setupDebugMessenger();
    pickDevice();
    createCommandPool();      // FIXME(OS) So far we need one GlobalPool and
    createCommandBuffers(1);  // 1 Command Buffer
    createSemaphores(1);   // FIXME(OS) We only need one Sempahore (+1 for Transfer in handler)
    prepareVma();
    createFences();
    // createCache();
    initBufferHandler();
    m_state = EngineStates_t::PRERUN;
}

/**
 * @brief Initializes the Buffer Handler
 * Call after 'init'
 *
 */
void VulkanEngine::initBufferHandler() {
    m_BufferHandler = new BufferHandler(m_Device, m_VmaAllocator, m_computeFamily, getStagingBufferSize());
    RAYX_D_LOG << "BufferHandler initialized.";
}

void VulkanEngine::createComputePipelinePass(const ComputePassCreateInfo& createInfo) {
    m_ComputePass = new ComputePass(m_Device, createInfo);
    RAYX_D_LOG << m_ComputePass->getName() << " ComputePipelinePass created.";
}

// Buffer Descriptor binding
void VulkanEngine::prepareComputePipelinePass() {
    auto bindings = getBufferHandler().getDescriptorBindings(m_ComputePass->getName());
    m_ComputePass->prepare(bindings);
    RAYX_D_LOG << "ComputePipelinePass prepared.";
}

}  // namespace RAYX

#endif
