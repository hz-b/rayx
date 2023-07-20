#ifndef NO_VULKAN

#include "VulkanEngine/Init/Fence.h"

#include "RayCore.h"
#include "VulkanEngine/Common.h"
namespace RAYX {
const uint64_t DEFAULT_TIMEOUT = 1000000000;  // 1 Second

Fence::Fence(VkDevice& device) : device(device) {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &f))
}

// Timeout : DEFAULT_TIMEOUT (1s)
// Fence is usable again after this call.
VkResult Fence::waitAndReset() {
    auto res = vkWaitForFences(device, 1, &f, VK_TRUE, DEFAULT_TIMEOUT);
    res = vkResetFences(device, 1, &f);
    return res;
}

VkFence* Fence::fence() { return &f; }

VkResult Fence::forceReset() {
    auto res = vkResetFences(device, 1, &f);
    return res;
}

Fence::~Fence() { vkDestroyFence(device, f, nullptr); }

}  // namespace RAYX

#endif
