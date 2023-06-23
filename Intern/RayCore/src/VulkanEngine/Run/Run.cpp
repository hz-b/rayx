#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::updateDescriptorSets(std::string passName) {
    // m_ComputePass->updateDescriptorSets(m_BufferHandler);
    getComputePass(passName)->simpleupdate(m_BufferHandler);
}

void VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        // RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;

    // TODO! Implement loop here
    // Using new descriptor manager
    updateDescriptorSets("PassName");
    getComputePass("PassName")
        ->getPass()[0]
        ->updatePushConstants(const_cast<void*>(m_pushConstants.pushConstPtr),
                              m_pushConstants.size);  // TODO : Why is the cast happeing?
    // prepareComputePipelinePass();
    recordInCommandBuffer(*getComputePass("PassName"), 0);
    submitCommandBuffer(0);
    m_runs++;
    m_state = EngineStates_t::POSTRUN;
}

}  // namespace RAYX

#endif
