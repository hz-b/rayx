#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {

struct Workgroup_t {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
};

Workgroup_t guessWorkGroupNo(uint32_t requiredLocalWorkGroupNo, VkPhysicalDeviceLimits limits) {
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

void VulkanEngine::recordInCommandBuffer(ComputePass& computePass, int cmdBufIndex) {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Recording new commandBuffer..";
    auto cmdBuffer = &m_CommandBuffers[cmdBufIndex];
    /*
    Now we shall start recording commands into the newly allocated command
    buffer.
    */
    VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK_RESULT(vkBeginCommandBuffer(*cmdBuffer, &beginInfo));  // start recording commands.

    /*
    We need to bind a pipeline, AND a descriptor set before we dispatch.
    The validation layer will NOT give warnings if you forget these, so be
    very careful not to forget them.
    */
    computePass.bind(*cmdBuffer, 0);  // TODO(OS) Add multi stage support if needed
    computePass.bindDescriptorSet(*cmdBuffer, 0);

    /*
    Calling vkCmdDispatch basically starts the compute pipeline, and
    executes the compute shader. The number of workgroups is specified in
    the arguments. If you are already familiar with compute shaders from
    OpenGL, this should be nothing new to you.
    */
    auto requiredLocalWorkGroupNo = (uint32_t)ceil(m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    // check if there are too many rays
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    auto group = guessWorkGroupNo(requiredLocalWorkGroupNo, deviceProperties.limits);
    /**
     * Update push constants
     */
    vkCmdPushConstants(*cmdBuffer, computePass.getPipelineLayout(0), VK_SHADER_STAGE_COMPUTE_BIT, 0,
                       computePass.getPass()[0]->m_pushConstant.getSize(), computePass.getPass()[0]->m_pushConstant.getData());

    RAYX_VERB << "Dispatching commandBuffer...";
    RAYX_VERB << "Sending "
              << "(" << group.x << ", " << group.z << ", " << group.y << ") to the GPU";
    vkCmdDispatch(*cmdBuffer, group.x, group.z, group.y);

    VK_CHECK_RESULT(vkEndCommandBuffer(*cmdBuffer));  // end recording commands.
}

void VulkanEngine::recordFirstCommand() {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Recording first new commandBuffer..";

    // TODO: ADD memory barrier if needed

    // First stage: PreLoopStage
    // -------------------------------------------------------------------------------------------------------
    auto pass0 = getComputePass("InitTracePass");
    auto cmdBuffer0 = &m_CommandBuffers[0];
    VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    // start recording commands
    VK_CHECK_RESULT(vkBeginCommandBuffer(*cmdBuffer0, &beginInfo));
    pass0->bind(*cmdBuffer0, 0);
    pass0->bindDescriptorSet(*cmdBuffer0, 0);

    auto requiredLocalWorkGroupNo = (uint32_t)ceil(m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    auto group = guessWorkGroupNo(requiredLocalWorkGroupNo, deviceProperties.limits);
    pass0->cmdPushConstants(*cmdBuffer0, 0);

    vkCmdDispatch(*cmdBuffer0, group.x, group.z, group.y);

    // Memory barrier (Init->Loop(i=0))
    m_BufferHandler->insertBufferMemoryBarrier("ray-buffer", *cmdBuffer0, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                               VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

    // Second sage: LoopBodyStage
    // -------------------------------------------------------------------------------------------------------
    pass0->bind(*cmdBuffer0, 1);
    pass0->bindDescriptorSet(*cmdBuffer0, 1);
    pass0->cmdPushConstants(*cmdBuffer0, 1);

    vkCmdDispatch(*cmdBuffer0, group.x, group.z, group.y);
    VK_CHECK_RESULT(vkEndCommandBuffer(*cmdBuffer0));  // end recording commands.

    // TODO: ADD memory barrier if needed
}

void VulkanEngine::recordSecondCommand() {
    RAYX_PROFILE_FUNCTION();
    RAYX_VERB << "Recording second new commandBuffer..";

    // TODO: add memory barrier
    auto pass1 = getComputePass("TracePass");
    auto cmdBuffer1 = &m_CommandBuffers[1];
    VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // start recording commands
    VK_CHECK_RESULT(vkBeginCommandBuffer(*cmdBuffer1, &beginInfo));
    pass1->bind(*cmdBuffer1, 0);
    pass1->bindDescriptorSet(*cmdBuffer1, 0);

    // auto traceCommand = [](VkCOmmandBuffer & command)

    auto requiredLocalWorkGroupNo = (uint32_t)ceil(m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    auto group = guessWorkGroupNo(requiredLocalWorkGroupNo, deviceProperties.limits);
    pass1->cmdPushConstants(*cmdBuffer1, 0);

    vkCmdDispatch(*cmdBuffer1, group.x, group.z, group.y);
    VK_CHECK_RESULT(vkEndCommandBuffer(*cmdBuffer1));  // end recording commands.
}

void VulkanEngine::traceCommand(VkCommandBuffer& cmdBuffer) {
    auto requiredLocalWorkGroupNo = (uint32_t)ceil(m_numberOfInvocations / float(WORKGROUP_SIZE));  // number of local works groups
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    auto group = guessWorkGroupNo(requiredLocalWorkGroupNo, deviceProperties.limits);
    vkCmdDispatch(cmdBuffer, group.x, group.z, group.y);
}

void VulkanEngine::recordSimpleTraceCommand(VkCommandBuffer& commandBuffer) {
    RAYX_PROFILE_FUNCTION();
    auto pass0 = getComputePass("singleTracePass");
    VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // start recording commands
    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    pass0->bind(commandBuffer, 0);
    pass0->bindDescriptorSet(commandBuffer, 0);
    pass0->cmdPushConstants(commandBuffer, 0);  // Bind push constants

    traceCommand(commandBuffer);
    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));  // end recording commands.
}

// template <typename Lambda, typename... Args>
// void VulkanEngine::recordCommand(std::string passName, VkCommandBuffer& commandBuffer, Lambda&& commandLambda, Args&&... args, int stage) {
//     RAYX_PROFILE_FUNCTION();
//     auto pass1 = getComputePass("TracePass");
//     VkCommandBufferBeginInfo beginInfo = VKINIT::Command::command_buffer_begin_info();
//     beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

//     // start recording commands
//     VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
//     pass1->bind(commandBuffer, 0);
//     pass1->bindDescriptorSet(commandBuffer, 0);

//     command(commandBuffer);  // Record command

//     pass1->cmdPushConstants(commandBuffer, 0);           // Bind push constants
//     VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));  // end recording commands.
// }
}  // namespace RAYX

#endif
