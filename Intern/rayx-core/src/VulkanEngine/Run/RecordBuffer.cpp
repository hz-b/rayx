#ifndef NO_VULKAN

#include <utility>

#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {

struct Workgroup_t {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
};

static Workgroup_t guessWorkGroupNo(uint32_t requiredLocalWorkGroupNo, VkPhysicalDeviceLimits limits) {
    static uint32_t requiredGroup = 0;
    static struct Workgroup_t group;

    auto xgroups = limits.maxComputeWorkGroupCount[0];
    auto ygroups = limits.maxComputeWorkGroupCount[1];
    auto zgroups = limits.maxComputeWorkGroupCount[2];

    // if this limit is reached, you may start using
    // multidimensional local workgroups.
    if (requiredLocalWorkGroupNo > xgroups * ygroups * zgroups) {
        RAYX_ERR << "the given task requires " << requiredLocalWorkGroupNo
                 << " many local work groups, but the maximal number on this "
                    "machine is "
                 << xgroups * ygroups * zgroups;
    } else {
        RAYX_VERB << "your machine supports up to " << xgroups * ygroups * zgroups * WORKGROUP_SIZE << " rays";
    }

    if (requiredGroup != requiredLocalWorkGroupNo) {
        // decrease xgroups, ygroups, zgroups so that we get a small number of
        // workgroups stlil covering requiredLocalWorkGroupNo
        {
            while (xgroups * ygroups * (zgroups / 2) >= requiredLocalWorkGroupNo) {
                zgroups /= 2;
            }
            while (xgroups * ygroups * (zgroups - 1) >= requiredLocalWorkGroupNo) {
                zgroups--;
            }

            while (xgroups * (ygroups / 2) * zgroups >= requiredLocalWorkGroupNo) {
                ygroups /= 2;
            }
            while (xgroups * (ygroups - 1) * zgroups >= requiredLocalWorkGroupNo) {
                ygroups--;
            }

            while ((xgroups / 2) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
                xgroups /= 2;
            }
            while ((xgroups - 1) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
                xgroups--;
            }
        }
        group.x = xgroups;
        group.y = ygroups;
        group.z = zgroups;
    }
    return group;
}

void VulkanEngine::traceCommand(VkCommandBuffer& cmdBuffer) {
    auto requiredLocalWorkGroupNo = (uint32_t)ceil((float)m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    auto group = guessWorkGroupNo(requiredLocalWorkGroupNo, deviceProperties.limits);
    RAYX_D_LOG << "Sending "
               << "(" << group.x << ", " << group.y << ", " << group.z << ") to the GPU";
    vkCmdDispatch(cmdBuffer, group.x, group.y, group.z);
}

void VulkanEngine::recordSimpleTraceCommand(std::string passName, VkCommandBuffer& commandBuffer, int stage) {
    RAYX_PROFILE_FUNCTION();
    auto pass = getComputePass(std::move(passName));
    VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // start recording commands
    checkVkResult(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    pass->bind(commandBuffer, stage);
    pass->bindDescriptorSet(commandBuffer, stage);
    pass->cmdPushConstants(commandBuffer, stage);  // Bind push constants

    traceCommand(commandBuffer);
    checkVkResult(vkEndCommandBuffer(commandBuffer));  // end recording commands.
}

}  // namespace RAYX

#endif
