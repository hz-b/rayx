#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::gpuMemcpy(VkBuffer& bufferDst, size_t offsetDst, VkBuffer& bufferSrc, size_t offsetSrc, size_t bytes) {
    RAYX_PROFILE_FUNCTION();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(m_TransferCommandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = offsetSrc;
    copyRegion.dstOffset = offsetDst;
    copyRegion.size = bytes;

    vkCmdCopyBuffer(m_TransferCommandBuffer, bufferSrc, bufferDst, 1, &copyRegion);

    vkEndCommandBuffer(m_TransferCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_TransferCommandBuffer;
    auto f = m_Fences.transfer->fence();
    vkQueueSubmit(m_TransferQueue, 1, &submitInfo, *f);
    // vkQueueWaitIdle(m_TransferQueue);
}

}  // namespace RAYX

#endif
