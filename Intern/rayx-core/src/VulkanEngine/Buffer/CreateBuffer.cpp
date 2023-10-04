#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::createBuffer(const char* buffName, VkDeviceSize size) {
    if (m_state == VulkanEngineStates::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == VulkanEngineStates::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }
    Buffer& b = m_buffers[buffName];

    // No need to recreate buffers if target is already there
    if (b.size == size && m_runs > 0) {
        return;
    }
    if (b.size != size && b.alloca != nullptr) {
        // Destroy current buffer to create a new one
        vmaDestroyBuffer(m_VmaAllocator, b.buf, b.alloca);
    }

    b.size = size;

    int buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (b.isInput) {
        buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (b.isOutput) {
        buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    // createVkBuffer(size, buffer_usage_flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, b.m_Buffer, b.m_Memory);
    createVmaBuffer(size, buffer_usage_flags, b.buf, b.alloca, &b.allocaInfo);
}

// find memory type with desired properties.
uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) {
    RAYX_PROFILE_FUNCTION();
    VkPhysicalDeviceMemoryProperties memoryProperties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    /*
    How does this search work?
    See the documentation of VkPhysicalDeviceMemoryProperties for a detailed
    description.
    */
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((memoryTypeBits & (1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) return i;
    }
    return (uint32_t)-1;
}
// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void VulkanEngine::createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkBuffer& buffer, VmaAllocation& allocation,
                                   VmaAllocationInfo* allocationInfo, VmaAllocationCreateFlags flags, VmaMemoryUsage memoryUsage,
                                   const std::vector<uint32_t>& queueFamilyIndices) {
    // Vulkan Buffer
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.size = size;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (queueFamilyIndices.size() >= 2) {
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        bufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    VmaAllocationCreateInfo VmaBufferAllocationMemoryInfo{};
    VmaBufferAllocationMemoryInfo.flags = flags;
    VmaBufferAllocationMemoryInfo.usage = memoryUsage;  // Defaults to Auto

    auto result = vmaCreateBuffer(m_VmaAllocator, &bufferCreateInfo, &VmaBufferAllocationMemoryInfo, &buffer, &allocation, allocationInfo);
    if (result != VK_SUCCESS) {
        RAYX_ERR << "Cannot Create Buffer of " << size << " bytes";
    }
}

}  // namespace RAYX

#endif
