#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::cleanup() {
    IS_ENGINE_CLEANABLE
    for (auto& pipeline : m_ComputePass->getPass()) {
        pipeline->cleanPipeline(m_Device);
    }
    m_state = EngineStates_t::PRERUN;
}

}  // namespace RAYX

#endif
