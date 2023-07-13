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
    createCommandPool();      // FIXME(OS) So far, we need one GlobalPool and
    createCommandBuffers(1);  // 1 Command Buffer
    createSemaphores(1);      // FIXME(OS) We only need one Semaphore (+1 for Transfer in handler)
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
 * @param createInfo Creation information
 */
void VulkanEngine::createComputePipelinePass(const ComputePassCreateInfo_t& createInfo) {
    auto pass = new ComputePass(m_Device, createInfo);
    m_computePasses.push_back(pass);
    RAYX_D_LOG << pass->getName() << " pass created.";
}

/**
 * @brief Prepare a compute Pass (Buffer Descriptor binding)
 *
 * @param index pass index
 */
void VulkanEngine::prepareComputePipelinePass(int index) {
    auto bindings = m_BufferHandler->getDescriptorBindings(m_computePasses[index]->getName());
    m_computePasses[index]->prepare(bindings);
}
/**
 * @brief Prepare a compute Pass (Buffer Descriptor binding)
 *
 * @param passName Compute Pass name (if exists)
 */
void VulkanEngine::prepareComputePipelinePass(std::string passName) {
    ComputePass* pass = getComputePass(passName);
    auto bindings = m_BufferHandler->getDescriptorBindings(pass->getName());
    pass->prepare(bindings);
}
/**
 * @brief Prepares all compute Passes (Buffer Descriptor binding)
 *
 */
void VulkanEngine::prepareComputePipelinePasses() {
    for (const auto pass : m_computePasses) {
        auto bindings = m_BufferHandler->getDescriptorBindings(pass->getName());
        pass->prepare(bindings);
    }
    RAYX_D_LOG << "ComputePipelinePass(s) prepared.";
}

void VulkanEngine::printPasses() {
    RAYX_D_LOG << "========PASSES========";
    for (const auto& p : m_computePasses) {
        std::string pipeNames = "";
        for (const auto& pass : p->getPass()) {
            pipeNames += pass->m_name + " ";
        }
        RAYX_D_LOG << p->getName() << "(" << p->getStageAmount() << "): " << pipeNames;
    }
    RAYX_D_LOG << "======================";
}

}  // namespace RAYX

#endif
