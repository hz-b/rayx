#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::cleanup() {
    if (m_state != EngineStates::POSTRUN) {
        RAYX_ERR << "cleanup() only needs to be called after .run()!";
    }

    for (auto pass : m_computePasses) {
        for (auto& pipeline : pass->getPipelines()) {
            pipeline->cleanPipeline(m_Device);
        }
    }
    m_state = EngineStates::PRERUN;
}

}  // namespace RAYX

#endif
