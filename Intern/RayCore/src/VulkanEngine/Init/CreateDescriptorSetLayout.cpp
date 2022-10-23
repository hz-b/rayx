#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::createDescriptorSetLayout() {
    RAYX_PROFILE_FUNCTION();

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto& [name, b] : m_buffers) {
        bindings.push_back({b.m_binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr});
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

}  // namespace RAYX
