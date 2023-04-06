#ifndef NO_VULKAN

#pragma once

#include <VulkanEngine/VulkanEngine.h>
#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

#include "RayCore.h"

namespace RAYX {

typedef enum BufferAccessFlags {
    VKBUFFER_IN,
    VKBUFFER_OUT,
    VKBUFFER_INOUT,
} BufferAccessFlags;

/**
 * @brief Pass to VulkanBuffer for buffer creation
 *
 */
struct VulkanBufferCreateInfo {
    // Unique Buffer name
    const char* bufName;
    // Access type to Buffer
    BufferAccessFlags accessType;
    // Size
    VkDeviceSize size;
    // used to map the vulkan buffers to the shader buffers.
    // Lines like `layout (binding = _)` declare buffers in the shader.
    uint32_t binding;
};

class VulkanEngine;
/**
 * @brief Vulkan Storage Buffer Class. Used for Buffer allocation, creation and update
 *
 */
class RAYX_API VulkanBuffer {
    friend class BufferHandler;

  public:
    VulkanBuffer() = default;
    VulkanBuffer(const VmaAllocator&, VulkanBufferCreateInfo createInfo);
    ~VulkanBuffer();

    VkDeviceSize getSize() const { return m_createInfo.size; }
    const VkBuffer& getBuffer() const { return m_Buffer; };
    const char* getName() const { return m_createInfo.bufName; }
    const VkDeviceMemory& GetBufferMemory() const { return m_Memory; }

    void* getMappedMemory();
    void UnmapMemory();

  private:
    // VMA Version of createVkBuffer
    void createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkBuffer& buffer, VmaAllocation& allocation,
                         VmaAllocationInfo* allocation_info, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                         VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO, const std::vector<uint32_t>& queue_family_indices = {});
    VmaAllocator m_VmaAllocator;
    VulkanBufferCreateInfo m_createInfo;
    VmaAllocationCreateFlags m_VmaFlags;
    VkBuffer m_Buffer = nullptr;
    VkDeviceMemory m_Memory = nullptr;
    VmaAllocation m_Alloca = nullptr;
    VmaAllocationInfo m_AllocaInfo;
};  // namespace RAYX

}  // namespace RAYX
#endif
