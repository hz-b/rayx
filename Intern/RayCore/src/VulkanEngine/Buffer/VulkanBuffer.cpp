#ifndef NO_VULKAN

#include "VulkanBuffer.h"

namespace RAYX {

inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags flags, uint32_t binding,
                                                               uint32_t count = 1) {
    VkDescriptorSetLayoutBinding set_layout_binding{};
    set_layout_binding.descriptorType = type;
    set_layout_binding.stageFlags = flags;
    set_layout_binding.binding = binding;
    set_layout_binding.descriptorCount = count;
    return set_layout_binding;
}
VulkanBuffer::VulkanBuffer(const VmaAllocator& vmaAllocator, VulkanBufferCreateInfo createInfo)
    : m_VmaAllocator(vmaAllocator), m_createInfo(createInfo) {
    int bufferUsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;  // Always storage buffer;
    if (createInfo.accessType == VKBUFFER_IN) {
        bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    } else if (createInfo.accessType == VKBUFFER_OUT) {
        bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    } else {
        bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    createVmaBuffer(m_createInfo.size, bufferUsageFlags, m_Buffer, m_Alloca, &m_AllocaInfo);

    // Defaults to compute buffers! TODO(OS): Once multiple pipelines are available implement more flagBITs!
    m_DescriptorSetLayoutBinding = descriptorSetLayoutBinding(createInfo.bufferType, VK_SHADER_STAGE_COMPUTE_BIT, createInfo.binding);
}

VulkanBuffer::~VulkanBuffer() { vmaDestroyBuffer(m_VmaAllocator, m_Buffer, m_Alloca); }

uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void VulkanBuffer::createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkBuffer& buffer, VmaAllocation& allocation,
                                   VmaAllocationInfo* allocation_info, VmaAllocationCreateFlags flags, VmaMemoryUsage memory_usage,
                                   const std::vector<uint32_t>& queue_family_indices) {
    // Vulkan Buffer
    VkBufferCreateInfo bufferCreateinfo{};
    bufferCreateinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateinfo.usage = buffer_usage;
    bufferCreateinfo.size = size;
    bufferCreateinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (queue_family_indices.size() >= 2) {
        bufferCreateinfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        bufferCreateinfo.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
        bufferCreateinfo.pQueueFamilyIndices = queue_family_indices.data();
    }

    VmaAllocationCreateInfo VmaBufferAllocationMemoryInfo{};
    VmaBufferAllocationMemoryInfo.flags = flags;
    VmaBufferAllocationMemoryInfo.usage = memory_usage;  // Defaults to Auto

    auto result = vmaCreateBuffer(m_VmaAllocator, &bufferCreateinfo, &VmaBufferAllocationMemoryInfo, &buffer, &allocation, allocation_info);
    if (result != VK_SUCCESS) {
        RAYX_ERR << "Cannot Create Buffer of " << size << " bytes";
    }
    m_VmaFlags = flags;
}

// // find memory type with desired properties.
// uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) {
//     RAYX_PROFILE_FUNCTION();
//     VkPhysicalDeviceMemoryProperties memoryProperties;

//     vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

//     /*
//     How does this search work?
//     See the documentation of VkPhysicalDeviceMemoryProperties for a detailed
//     description.
//     */
//     for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
//         if ((memoryTypeBits & (1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) return i;
//     }
//     return (uint32_t)-1;
// }

void* VulkanBuffer::getMappedMemory() {
    void* buf;
    if (m_VmaFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) {
        buf = m_AllocaInfo.pMappedData;
    } else {
        vmaMapMemory(m_VmaAllocator, m_Alloca, &buf);
    }
    return buf;
}
void VulkanBuffer::UnmapMemory() {
    if (m_VmaFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) {
        ;
    } else {
        vmaUnmapMemory(m_VmaAllocator, m_Alloca);
    }
    return;
}

}  // namespace RAYX
#endif