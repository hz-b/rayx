#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/* Descriptor sets need a pool to allocate from, which is created here. */
void VulkanEngine::createDescriptorSet() {
    RAYX_PROFILE_FUNCTION();
    /*
    one descriptor for each buffer
    */
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type                 = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSize.descriptorCount      = m_buffers.size();  // = number of buffers

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets                    = 1;  // we need to allocate one descriptor sets from the pool.
    descriptorPoolCreateInfo.poolSizeCount              = 1;
    descriptorPoolCreateInfo.pPoolSizes                 = &descriptorPoolSize;

    // create descriptor pool.
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));

    /*
    With the pool allocated, we can now allocate the descriptor set.
    */
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool              = m_DescriptorPool;  // pool to allocate from.
    descriptorSetAllocateInfo.descriptorSetCount          = 1;                 // allocate a single descriptor set.
    descriptorSetAllocateInfo.pSetLayouts                 = &m_DescriptorSetLayout;

    // allocate descriptor set.
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &m_DescriptorSet));

    // Populate every Compute Buffer/Descriptor and configure it.
    for (auto& [name, b] : m_buffers) {
        // specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer                 = b.m_Buffer;
        descriptorBufferInfo.offset                 = 0;
        descriptorBufferInfo.range                  = b.m_size;

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext                = nullptr;
        writeDescriptorSet.dstSet               = m_DescriptorSet;  // write to this descriptor set.
        writeDescriptorSet.dstBinding           = b.m_binding;
        writeDescriptorSet.dstArrayElement      = 0;
        writeDescriptorSet.descriptorCount      = 1;                                  // update a single descriptor.
        writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo          = &descriptorBufferInfo;

        // perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
    }
}

}  // namespace RAYX
