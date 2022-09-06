#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void createBuffer(VkPhysicalDevice&, VkDevice&, VkDeviceSize size,
                  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer& buffer, VkDeviceMemory& bufferMemory);
uint32_t findMemoryType(VkPhysicalDevice& physicalDevice,
                        uint32_t memoryTypeBits,
                        VkMemoryPropertyFlags properties);

void VulkanEngine::createBuffers(RunSpec r) {
    RAYX_PROFILE_FUNCTION();

    for (const auto& [name, size] : r.buffersizes) {
        BufferSpec spec = m_initSpec->bufferSpecs[name];
        int buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (spec.in) {
            buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if (spec.out) {
            buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        InternalBuffer* ib = &m_internalBuffers[name];
        createBuffer(m_PhysicalDevice, m_Device, size, buffer_usage_flags,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ib->m_Buffer,
                     ib->m_Memory);
    }

    // create staging buffer
    createBuffer(m_PhysicalDevice, m_Device, STAGING_SIZE,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 m_stagingBuffer, m_stagingMemory);
}

// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void createBuffer(VkPhysicalDevice& physicalDevice, VkDevice& device,
                  VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer& buffer,
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
        vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize =
        memoryRequirements.size;  // specify required memory.
    allocateInfo.memoryTypeIndex = findMemoryType(
        physicalDevice, memoryRequirements.memoryTypeBits, properties);
    VK_CHECK_RESULT(
        vkAllocateMemory(device, &allocateInfo, NULL,
                         &bufferMemory));  // allocate memory on device.

    // Now associate that allocated memory with the buffer. With that, the
    // buffer is backed by actual memory.
    VK_CHECK_RESULT(vkBindBufferMemory(device, buffer, bufferMemory, 0));
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
