#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::cleanup() {
    IS_ENGINE_CLEANABLE

    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

    m_state = EngineStates_t::PRERUN;
}

void VulkanEngine::newCleanup() {
    IS_ENGINE_CLEANABLE
    for (auto& pipeline : m_ComputePass->getPass()) {
        pipeline->cleanPipeline(m_Device);
    }
    m_state = EngineStates_t::PRERUN;
}

}  // namespace RAYX

#endif
