#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {

// Function to calculate the optimal local workgroup dimensions
void calculateLocalWorkGroups(uint32_t requiredLocalWorkGroupNo, VkPhysicalDeviceProperties& deviceProperties, uint32_t& xgroups, uint32_t& ygroups,
                              uint32_t& zgroups) {
    auto limits = deviceProperties.limits;

    // Initial values
    xgroups = limits.maxComputeWorkGroupCount[0];
    ygroups = limits.maxComputeWorkGroupCount[1];
    zgroups = limits.maxComputeWorkGroupCount[2];

    // decrease xgroups, ygroups, zgroups so that we get a small number of
    // workgroups until covering requiredLocalWorkGroupNo
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

/*
 * Record Commands for Compute and Dispatch
 */
void VulkanEngine::recordInComputeCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Recording commandBuffer..";
    static uint32_t requiredGroup = 0;
    static struct {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
    } group;
    /*
    Now we shall start recording commands into the newly allocated command
    buffer.
    */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK_RESULT(vkBeginCommandBuffer(m_ComputeCommandBuffer, &beginInfo))  // start recording commands.

    /*
    We need to bind a pipeline, AND a descriptor set before we dispatch.
    The validation layer will NOT give warnings if you forget these, so be
    very careful not to forget them.
    */
    vkCmdBindPipeline(m_ComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
    vkCmdBindDescriptorSets(m_ComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
    // vkCmdBindDescriptorSets(commandBuffer,
    // VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1,
    // &descriptorSets[1], 0, NULL);

    /*
    Calling vkCmdDispatch basically starts the compute pipeline, and
    executes the compute shader. The number of workgroups is specified in
    the arguments. If you are already familiar with compute shaders from
    OpenGL, this should be nothing new to you.
    */
    auto requiredLocalWorkGroupNo = (uint32_t)ceil((float)m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    // check if there are too many rays
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    uint32_t xgroups, ygroups, zgroups;
    calculateLocalWorkGroups(requiredLocalWorkGroupNo, deviceProperties, xgroups, ygroups, zgroups);

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
        group.x = xgroups;
        group.y = ygroups;
        group.z = zgroups;
        requiredGroup = requiredLocalWorkGroupNo;
    }

    /**
     * Update push constants
     */
    vkCmdPushConstants(m_ComputeCommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, m_pushConstantsData.size,
                       m_pushConstantsData.pushConstPtr);

    RAYX_VERB << "Dispatching commandBuffer...";
    RAYX_VERB << "Sending "
              << "(" << group.x << ", " << group.z << ", " << group.y << ") to the GPU";
    vkCmdDispatch(m_ComputeCommandBuffer, group.x, group.z, group.y);

    VK_CHECK_RESULT(vkEndCommandBuffer(m_ComputeCommandBuffer))  // end recording commands.
}

}  // namespace RAYX

#endif
