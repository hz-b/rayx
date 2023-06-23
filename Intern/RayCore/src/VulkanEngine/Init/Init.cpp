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
    createSemaphores(1);      // FIXME(OS) We only need one Sempahore (+1 for Transfer in handler)
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

ComputePass* VulkanEngine::getComputePass(std::string passName) {
    for (auto p : m_computePasses) {
        if (p->getName() == passName) {
            return p;
        }
    }
    RAYX_ERR << "Pass " << passName << " not found.";
    return nullptr;
}
/**
 * @brief Adds a new Compute Pass to the engine
 *
 * @param createInfo
 */
void VulkanEngine::createComputePipelinePass(const ComputePassCreateInfo& createInfo) {
    auto pass = new ComputePass(m_Device, createInfo);
    m_computePasses.push_back(pass);
    RAYX_D_LOG << pass->getName() << " ComputePipelinePass created.";
}

/**
 * @brief Prepare a compute Pass (Buffer Descriptor binding)
 *
 * @param index pass index
 */
void VulkanEngine::prepareComputePipelinePass(int index) {
    auto bindings = getBufferHandler().getDescriptorBindings(m_computePasses[index]->getName());
    m_computePasses[index]->prepare(bindings);
    RAYX_D_LOG << "ComputePipelinePass prepared.";
}
/**
 * @brief Prepare a compute Pass (Buffer Descriptor binding)
 *
 * @param passName Compute Pass name (if exists)
 */
void VulkanEngine::prepareComputePipelinePass(std::string passName) {
    ComputePass* pass = getComputePass(passName);
    auto bindings = getBufferHandler().getDescriptorBindings(pass->getName());
    pass->prepare(bindings);
    RAYX_D_LOG << "ComputePipelinePass prepared.";
}
/**
 * @brief  Prepares all compute Passes (Buffer Descriptor binding)
 *
 */
void VulkanEngine::prepareComputePipelinePasses() {
    for (auto pass : m_computePasses) {
        auto bindings = getBufferHandler().getDescriptorBindings(pass->getName());
        pass->prepare(bindings);
    }
    RAYX_D_LOG << "ComputePipelinePass(s) prepared.";
}

}  // namespace RAYX

#endif
