#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <map>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"

namespace RAYX {

typedef enum BufferAccessFlags {
    VKBUFFER_IN,
    VKBUFFER_OUT,
    VKBUFFER_INOUT,
} BufferAccessFlags;

/**
 * @brief Pass to VulkanBuffer Class for buffer creation
 *
 */
struct VulkanBufferCreateInfo {
    const char* bufName;                                              // Unique Buffer name
    BufferAccessFlags accessType;                                     // Access type to Buffer
    VkDeviceSize size = 0;                                            // Size
    VkDescriptorType bufferType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // What kind of buffer is it (Storage, Image, Uniform etc.)
};

class BufferHandler;

// class VulkanEngine;
/**
 * @brief Vulkan Storage Buffer Class. Used for Buffer allocation, creation and update
 *
 */
class RAYX_API VulkanBuffer {
    friend class BufferHandler;

  public:
    VulkanBuffer(const VmaAllocator&, VulkanBufferCreateInfo createInfo);
    ~VulkanBuffer();

    // TODO(OS): Move/Copy constructor maybe needed
    // VulkanBuffer(const VulkanBuffer&) = delete;
    // VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkDeviceSize getSize() const { return m_createInfo.size; }
    const VkBuffer& getBuffer() const { return m_Buffer; };
    const char* getName() const { return m_createInfo.bufName; }
    const VkDeviceMemory& GetBufferMemory() const { return m_Memory; }

    void* getMappedMemory();
    void UnmapMemory();

    void addDescriptorSetPerPassBinding(const std::string& passName, uint32_t binding, VkShaderStageFlags shaderStageFlag);
    VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize offset = 0);
    uint32_t getPassDescriptorBinding(std::string passName) { return m_DescriptorSetBindings[passName].binding; }

  private:
    // VMA Version of createVkBuffer
    void createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkBuffer& buffer, VmaAllocation& allocation,
                         VmaAllocationInfo* allocation_info, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                         VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO, const std::vector<uint32_t>& queue_family_indices = {});

    VmaAllocator m_VmaAllocator;
    VulkanBufferCreateInfo m_createInfo;
    VmaAllocationCreateFlags m_VmaFlags;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VmaAllocation m_Alloca = VK_NULL_HANDLE;
    VmaAllocationInfo m_AllocaInfo;
    std::map<std::string, VkDescriptorSetLayoutBinding> m_DescriptorSetBindings;
};

}  // namespace RAYX
#endif
