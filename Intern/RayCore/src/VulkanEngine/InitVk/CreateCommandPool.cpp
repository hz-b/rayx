#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::createCommandPool() {
    RAYX_PROFILE_FUNCTION();
    /*
    In order to send commands to the device(GPU),
    we must first record commands into a command buffer.
    To allocate a command buffer, we must first create a command pool. So
    let us do that.
    */
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    // the queue family of this command pool. All command buffers allocated
    // from this command pool, must be submitted to queues of this family
    // ONLY.
    commandPoolCreateInfo.queueFamilyIndex = m_QueueFamily.computeFamily;
    VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &commandPoolCreateInfo,
                                        nullptr, &m_CommandPool));
}

}  // namespace RAYX
