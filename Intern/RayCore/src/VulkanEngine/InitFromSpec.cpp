#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::initFromSpec(InitSpec i) {
    RAYX_PROFILE_FUNCTION();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = i.size();
    descriptorSetLayoutCreateInfo.pBindings = i.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(
        vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo,
                                    nullptr, &m_DescriptorSetLayout));
}

}  // namespace RAYX
