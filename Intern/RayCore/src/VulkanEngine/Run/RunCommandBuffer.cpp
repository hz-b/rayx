#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief Submit/Send Commands to Device through Queue
 *  NO Waitfor fence!
 *
 */
void VulkanEngine::submitCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    /*
    Now we shall finally submit the recorded command buffer to a queue.
    */
    VkSubmitInfo submitInfo = VKINIT::misc::submit_info();
    submitInfo.commandBufferCount = 1;                     // submit a single command buffer
    submitInfo.pCommandBuffers = &m_ComputeCommandBuffer;  // the command buffer to submit.
    VK_CHECK_RESULT(m_Fences.compute->forceReset())
    auto f = m_Fences.compute->fence();
    /*
    We submit the command buffer on the queue, at the same time giving a
    fence. (Fences are like interrupts and used for async computations)
    */
    VK_CHECK_RESULT(vkQueueSubmit(m_ComputeQueue, 1, &submitInfo, *f));
    /*
    The command will not have finished executing until the fence is
    signaled. So we wait here. Directly afer this, we read our buffer
    from the GPU. Fences give us a hint that the Command in the Queue is
    actually done executing.
    */
    m_Fences.compute->wait();
}

}  // namespace RAYX

#endif
