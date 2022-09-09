#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::createCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    RAYX_LOG << "Creating commandBuffer..";
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool =
        m_CommandPool;  // specify the command pool to allocate from.

    /* if the command buffer is primary, it can be directly submitted to
    / queues. A secondary buffer has to be called from some primary command
    / buffer, and cannot be directly submitted to a queue. To keep things
    / simple, we use a primary command buffer. */

    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount =
        1;  // allocate a single command buffer.
    VK_CHECK_RESULT(vkAllocateCommandBuffers(
        m_Device, &commandBufferAllocateInfo,
        &m_CommandBuffer));  // allocate command buffer.

    /*
    Now we shall start recording commands into the newly allocated command
    buffer.
    */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  // the buffer is only
                                                      // submitted and used
                                                      // once in this
                                                      // application.
    VK_CHECK_RESULT(vkBeginCommandBuffer(
        m_CommandBuffer, &beginInfo));  // start recording commands.

    /*
    We need to bind a pipeline, AND a descriptor set before we dispatch.
    The validation layer will NOT give warnings if you forget these, so be
    very careful not to forget them.
    */
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      m_Pipeline);
    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            m_PipelineLayout, 0, 1, &m_DescriptorSet, 0,
                            nullptr);
    // vkCmdBindDescriptorSets(commandBuffer,
    // VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1,
    // &descriptorSets[1], 0, NULL);

    /*
    Calling vkCmdDispatch basically starts the compute pipeline, and
    executes the compute shader. The number of workgroups is specified in
    the arguments. If you are already familiar with compute shaders from
    OpenGL, this should be nothing new to you.
    */
    auto requiredLocalWorkGroupNo =
        (uint32_t)ceil(m_numberOfInvocations /
                       float(WORKGROUP_SIZE));  // number of local works groups

    // check if there are too many rays
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    auto limits = deviceProperties.limits;

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
        RAYX_D_LOG << "your machine supports up to "
                   << xgroups * ygroups * zgroups * WORKGROUP_SIZE << " rays";
    }

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

    RAYX_LOG << "Dispatching commandBuffer...";
    RAYX_D_LOG << "Sending "
               << "(" << xgroups << ", " << ygroups << ", " << zgroups
               << ") to the GPU";
    vkCmdDispatch(m_CommandBuffer, xgroups, ygroups, zgroups);

    VK_CHECK_RESULT(
        vkEndCommandBuffer(m_CommandBuffer));  // end recording commands.
}

}  // namespace RAYX
