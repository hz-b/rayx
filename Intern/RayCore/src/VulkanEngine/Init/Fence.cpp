#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
VulkanEngine::Fence::Fence(VkDevice& device) : device(device) {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &f));
}
// Timeout ~1 sec
// Fence is usable again after this.
VkResult VulkanEngine::Fence::wait() {
    auto res = vkWaitForFences(device, 1, &f, VK_TRUE, 1000000000);
    res = vkResetFences(device, 1, &f);
    return res;
}
VkResult VulkanEngine::Fence::forceReset() {
    auto res = vkResetFences(device, 1, &f);
    return res;
}

VulkanEngine::Fence::~Fence() { vkDestroyFence(device, f, nullptr); }

void VulkanEngine::createFences() {
    m_Fences.computeFence = std::make_unique<Fence>(m_Device);
    m_Fences.transferFence = std::make_unique<Fence>(m_Device);
}

}  // namespace RAYX