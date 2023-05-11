#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

/**
 * @brief Create Descriptor Set Layout with correct bindings (Storage in Compute)
 *
 */
void VulkanEngine::createDescriptorSetLayout() {
    RAYX_PROFILE_FUNCTION();

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto& [name, b] : m_buffers) {
        bindings.push_back({b.binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr});
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

/**
 * @brief Create Descriptor Pool and allocate Descriptor Set from it
 *
 * @param size Size of expected descriptors from the pool
 */
void VulkanEngine::createAllocateDescriptorPool(uint32_t size) {
    RAYX_PROFILE_FUNCTION();
    /*
    one descriptor for each buffer
    */
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSize.descriptorCount = size;  // = number of buffers

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets = 1;  // we need to allocate one descriptor sets from the pool.
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    // create descriptor pool.
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));

    /*
    With the pool allocated, we can now allocate the descriptor set.
    */
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;  // pool to allocate from.
    descriptorSetAllocateInfo.descriptorSetCount = 1;             // allocate a single descriptor set.
    descriptorSetAllocateInfo.pSetLayouts = &m_DescriptorSetLayout;

    // allocate descriptor set.
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &m_DescriptorSet));
}

}  // namespace RAYX

#endif
