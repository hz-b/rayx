#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
    
void VulkanEngine::newUpdateDescriptorSets() {
    m_ComputePass->simpleupdate(m_BufferHandler);
}

void VulkanEngine::newRun(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;

    // Using new descriptor manager
    newUpdateDescriptorSets();
    m_ComputePass->getPass()[0]->updatePushConstants(const_cast<void*>(m_pushConstants.pushConstPtr),
                                                     m_pushConstants.size);  // TODO : Why is the cast happeing?
    // prepareComputePipelinePass();
    newRecordInCommandBuffer(*m_ComputePass);
    submitCommandBuffer();
    m_runs++;
    m_state = EngineStates_t::POSTRUN;
}

}  // namespace RAYX

#endif
