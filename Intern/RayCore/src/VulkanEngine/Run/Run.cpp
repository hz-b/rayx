#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == VulkanEngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == VulkanEngineStates_t::POSTRUN) {
        RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }

    m_numberOfInvocations = spec.m_numberOfInvocations;

    updteDescriptorSets();
    createComputePipeline();
    recordInComputeCommandBuffer();
    submitCommandBuffer();

    m_state = VulkanEngineStates_t::POSTRUN;
}

}  // namespace RAYX
