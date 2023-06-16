#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::updateDescriptorSets() {
    //m_ComputePass->updateDescriptorSets(m_BufferHandler);
    m_ComputePass->simpleupdate(m_BufferHandler);
}

void VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        // RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;

    // Using new descriptor manager
    updateDescriptorSets();
    m_ComputePass->getPass()[0]->updatePushConstants(const_cast<void*>(m_pushConstants.pushConstPtr),
                                                     m_pushConstants.size);  // TODO : Why is the cast happeing?
    // prepareComputePipelinePass();
    recordInCommandBuffer(*m_ComputePass, 0);
    submitCommandBuffer(0);
    m_runs++;
    m_state = EngineStates_t::POSTRUN;
}

}  // namespace RAYX

#endif
