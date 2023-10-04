#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::run(VulkanEngineRunSpec spec) {
    if (m_state == EngineStates::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == EngineStates::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }

    m_numberOfInvocations = spec.m_numberOfInvocations;

    updateDescriptorSets();
    createComputePipeline();
    recordInComputeCommandBuffer();
    submitCommandBuffer();
    m_runs++;

    m_state = EngineStates::POSTRUN;
}

}  // namespace RAYX

#endif
