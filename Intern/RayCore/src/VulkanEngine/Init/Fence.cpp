#ifndef NO_VULKAN

#include "VulkanEngine/Init/Fence.h"

#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {
const uint64_t DEFAULT_TIMEOUT = 1000000000;  // 1 Second

// VulkanEngine::Fence::Fence(VkDevice& device) : device(device) {
//     VkFenceCreateInfo fenceCreateInfo = {};
//     fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//     fenceCreateInfo.flags = 0;
//     VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &f));
// }

// // Timeout ~1 sec
// // Fence is usable again after this.
// VkResult VulkanEngine::Fence::wait() {
//     auto res = vkWaitForFences(device, 1, &f, VK_TRUE, DEFAULT_TIMEOUT);
//     res = vkResetFences(device, 1, &f);
//     return res;
// }

// VkFence* VulkanEngine::Fence::fence() { return &f; }

// VkResult VulkanEngine::Fence::forceReset() {
//     auto res = vkResetFences(device, 1, &f);
//     return res;
// }

// VulkanEngine::Fence::~Fence() { vkDestroyFence(device, f, nullptr); }

////////////////////////////////////////////////////////////////////////////

Fence::Fence(VkDevice& device) : device(device) {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &f));
}

// Timeout ~1 sec
// Fence is usable again after this.
VkResult Fence::wait() {
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

void VulkanEngine::createFences() {
    m_Fences.compute = std::make_unique<Fence>(m_Device);
    m_Fences.transfer = std::make_unique<Fence>(m_Device);
}

}  // namespace RAYX

#endif
