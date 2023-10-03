#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::cleanup() {
    if (m_state != VulkanEngineStates::POSTRUN) {
        RAYX_ERR << "cleanup() only needs to be called after .run()!";
    }

    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

    m_state = VulkanEngineStates::PRERUN;
}

}  // namespace RAYX

#endif
