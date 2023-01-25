#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::gpuMemcpy(VkBuffer& buffer_dst, size_t offset_dst, VkBuffer& buffer_src, size_t offset_src, size_t bytes) {
    RAYX_PROFILE_FUNCTION();
    // VkCommandBufferAllocateInfo allocInfo{};
    // allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // allocInfo.commandPool = m_CommandPool;
    // allocInfo.commandBufferCount = 1;

    // VkCommandBuffer commandBuffer;
    // vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(m_TransferCommandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = offset_src;
    copyRegion.dstOffset = offset_dst;
    copyRegion.size = bytes;

    vkCmdCopyBuffer(m_TransferCommandBuffer, buffer_src, buffer_dst, 1, &copyRegion);

    vkEndCommandBuffer(m_TransferCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_TransferCommandBuffer;

    vkQueueSubmit(m_ComputeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_ComputeQueue);

    //vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
}

}  // namespace RAYX
