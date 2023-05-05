#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief Creates Command Pool for Command Buffers
 *
 */
void VulkanEngine::createCommandPool() {
    RAYX_PROFILE_FUNCTION();
    /*
    In order to send commands to the device(GPU),
    we must first record commands into a command buffer.
    To allocate a command buffer, we must first create a command pool. So
    let us do that.
    */
    auto commandPoolCreateInfo = VKINIT::Command::command_pool_create_info();

    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // the queue family of this command pool. All command buffers allocated
    // from this command pool, must be submitted to queues of this family
    // ONLY.
    commandPoolCreateInfo.queueFamilyIndex = m_computeFamily;
    VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_GlobalCommandPool));
}

}  // namespace RAYX

#endif
