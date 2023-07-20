#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * Updte Descriptor Set's Buffers and range with given buffers.(m_buffers)
 * You should call this whenever content is being updated
 */
void VulkanEngine::updteDescriptorSets() {
    RAYX_PROFILE_FUNCTION();

    // Populate every Compute Buffer/Descriptor and configure it.
    for (auto& [name, b] : m_buffers) {
        // specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = b.buf;
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = b.size;

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_DescriptorSet;  // write to this descriptor set.
        writeDescriptorSet.dstBinding = b.binding;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;                                 // update a single descriptor.
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

        // perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
    }
}

}  // namespace RAYX

#endif
