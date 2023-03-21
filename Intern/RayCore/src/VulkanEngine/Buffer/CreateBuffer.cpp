#ifndef NO_VULKAN

#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::createBuffer(const char* bufname, VkDeviceSize size) {
    if (m_state == VulkanEngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    } else if (m_state == VulkanEngineStates_t::POSTRUN) {
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    }
    Buffer_t& b = m_buffers[bufname];

    // No need to recreate buffers if target is already there
    if (b.size == size && m_runs > 0) {
        return;
    } else if (b.size != size && b.alloca != nullptr) {
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

uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void VulkanEngine::createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                  VkDeviceMemory& bufferMemory) {
    RAYX_PROFILE_FUNCTION();
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;                            // buffer is used as a storage buffer.
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // buffer is exclusive to a single
                                                               // queue family at a time.
    VK_CHECK_RESULT(vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &buffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;  // specify required memory.
    allocateInfo.memoryTypeIndex = findMemoryType(m_PhysicalDevice, memoryRequirements.memoryTypeBits, properties);
    VK_CHECK_RESULT(vkAllocateMemory(m_Device, &allocateInfo, NULL,
                                     &bufferMemory));  // allocate memory on device.

    // Now associate that allocated memory with the buffer. With that, the
    // buffer is backed by actual memory.
    VK_CHECK_RESULT(vkBindBufferMemory(m_Device, buffer, bufferMemory, 0));
}

void VulkanEngine::createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkBuffer& buffer, VmaAllocation& allocation,
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

}  // namespace RAYX

#endif
