#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::initPipeline(Pipeline p) {
    RAYX_PROFILE_FUNCTION();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = p.size();
    descriptorSetLayoutCreateInfo.pBindings = p.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(
        vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo,
                                    nullptr, &m_DescriptorSetLayout));
}

}  // namespace RAYX
