#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::cleanup() {
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffer);
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

    for (auto& [name, buf] : m_buffers) {
        vkDestroyBuffer(m_Device, buf.m_Buffer, nullptr);
        vkFreeMemory(m_Device, buf.m_Memory, nullptr);
    }

    vkDestroyShaderModule(m_Device, m_ComputeShaderModule, nullptr);
}

}  // namespace RAYX
