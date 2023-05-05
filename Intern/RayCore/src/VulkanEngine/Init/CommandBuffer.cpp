#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

/**
 * @brief Creates 1 CommandBuffer that automatically starts recording.
 * Warning: ONETIME COMMANDBUFFER! Not meant to record, end multiple times --> Invalid State
 *
 * @return VkCommandBuffer
 */
VkCommandBuffer VulkanEngine::createOneTimeCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Creating one time commandBuffer..";
    VkCommandBuffer cmdBuffer;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_GlobalCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &cmdBuffer));

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK_RESULT(vkBeginCommandBuffer(m_ComputeCommandBuffer, &beginInfo));
    return cmdBuffer;
}
/**
 * @brief Create and allocate required command buffers (Compute and Transfer)
 *
 */
void VulkanEngine::createCommandBuffers() {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Creating commandBuffers..";
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_GlobalCommandPool;  // specify the command pool to allocate from.

    /* if the command buffer is primary, it can be directly submitted to
    / queues. A secondary buffer has to be called from some primary command
    / buffer, and cannot be directly submitted to a queue. To keep things
    / simple, we use a primary command buffer. */
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;  // allocate a single command buffer.
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo,
                                             &m_ComputeCommandBuffer));  // allocate command buffer.

    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_TransferCommandBuffer));
}

void VulkanEngine::createCommandBuffers(int commandBuffersCount) {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Creating commandBuffers..";

    m_CommandBuffers.reserve(commandBuffersCount);
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_GlobalCommandPool;  // specify the command pool to allocate from.

    for (auto i = 0; i < commandBuffersCount; i++) {
        /* if the command buffer is primary, it can be directly submitted to
        / queues. A secondary buffer has to be called from some primary command
        / buffer, and cannot be directly submitted to a queue. To keep things
        / simple, we use a primary command buffer. */
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;  // allocate a single command buffer.
        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo,
                                                 &m_CommandBuffers[i]));  // allocate command buffer.
    }
}

}  // namespace RAYX

#endif
