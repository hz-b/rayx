#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::postRunCleanup() {
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1,
                         &m_CommandBuffer);
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout,
                            nullptr);
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool,
                            nullptr);

	for (auto& [name, buf] : m_internalBuffers) {
        vkDestroyBuffer(m_Device, buf.m_Buffer, nullptr);
        vkFreeMemory(m_Device, buf.m_Memory, nullptr);
    }

	vkDestroyBuffer(m_Device, m_stagingBuffer, nullptr);
	vkFreeMemory(m_Device, m_stagingMemory, nullptr);

    vkDestroyShaderModule(m_Device, m_ComputeShaderModule,
                          nullptr);
}

}
