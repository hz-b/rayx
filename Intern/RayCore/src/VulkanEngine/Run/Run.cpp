#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::run(RunSpec spec) {
    if (m_state == EngineState::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineState::POSTRUN) {
        RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }

    m_numberOfInvocations = spec.m_numberOfInvocations;

    createDescriptorSet();
    createComputePipeline();
    createCommandBuffer();
    runCommandBuffer();

    m_state = EngineState::POSTRUN;
}

bool VulkanEngine::setSpecializationData(SpecializationData spData) {
    if (m_state != EngineState::POSTRUN) {
        m_specilizationData = spData;
        return true;
    } else {
        RAYX_WARN << "No effect";
        return false;
    }
}

}  // namespace RAYX
