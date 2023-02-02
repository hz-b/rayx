#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::gpuMemcpy(VkBuffer& buffer_dst, size_t offset_dst, VkBuffer& buffer_src, size_t offset_src, size_t bytes) {
    RAYX_PROFILE_FUNCTION();
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

    vkQueueSubmit(m_TransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_TransferQueue);
}

}  // namespace RAYX
