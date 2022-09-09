#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

uint32_t findMemoryType(VkPhysicalDevice& physicalDevice,
                        uint32_t memoryTypeBits,
                        VkMemoryPropertyFlags properties);


// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void VulkanEngine::createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer& buffer,
                                  VkDeviceMemory& bufferMemory) {
    RAYX_PROFILE_FUNCTION();
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;  // buffer is used as a storage buffer.
    bufferCreateInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;  // buffer is exclusive to a single
                                    // queue family at a time.
    VK_CHECK_RESULT(
        vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &buffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize =
        memoryRequirements.size;  // specify required memory.
    allocateInfo.memoryTypeIndex = findMemoryType(
        m_PhysicalDevice, memoryRequirements.memoryTypeBits, properties);
    VK_CHECK_RESULT(
        vkAllocateMemory(m_Device, &allocateInfo, NULL,
                         &bufferMemory));  // allocate memory on device.

    // Now associate that allocated memory with the buffer. With that, the
    // buffer is backed by actual memory.
    VK_CHECK_RESULT(vkBindBufferMemory(m_Device, buffer, bufferMemory, 0));
}

// find memory type with desired properties.
uint32_t findMemoryType(VkPhysicalDevice& physicalDevice,
                        uint32_t memoryTypeBits,
                        VkMemoryPropertyFlags properties) {
    RAYX_PROFILE_FUNCTION();
    VkPhysicalDeviceMemoryProperties memoryProperties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    /*
    How does this search work?
    See the documentation of VkPhysicalDeviceMemoryProperties for a detailed
    description.
    */
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & properties) ==
             properties))
            return i;
    }
    return -1;
}

}  // namespace RAYX
