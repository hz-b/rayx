#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <map>
#include <vulkan/vulkan.hpp>

#include "RAY-Core.h"

namespace RAYX {
enum BufferAccessFlags {
    VKBUFFER_IN,
    VKBUFFER_OUT,
    VKBUFFER_INOUT,
};

struct VulkanBufferCreateInfo {
    const char* bufName;                                              // Unique Buffer name
    BufferAccessFlags accessType;                                     // Access type to Buffer
    VkDeviceSize size = 0;                                            // Size
    VkDescriptorType bufferType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // What kind of buffer is it (Storage [Default], Image, Uniform etc.)
};

struct PassBindingInfo {
    std::string passName;
    uint32_t binding;
    VkShaderStageFlags shaderStageFlag;
};

class BufferHandler;

/**
 * @brief Vulkan Storage Buffer Class, used for Buffer allocation, creation and update
 *
 */
class RAYX_API VulkanBuffer {
    friend class BufferHandler;

  public:
    /**
     * @brief Constructs a new Vulkan Buffer (with VMA-on-GPU allocation if size is different than 0!)
     *
     * @param createInfo
     */
    VulkanBuffer(const VmaAllocator&, VulkanBufferCreateInfo createInfo);
    ~VulkanBuffer();

    // Move/Copy of a VulkanBuffer are not alloawed
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkDeviceSize getSize() const { return m_createInfo.size; }
    const VkBuffer& getBuffer() const { return m_Buffer; }
    const char* getName() const { return m_createInfo.bufName; }
    const VkDeviceMemory& GetBufferMemory() const { return m_Memory; }
    bool hasPassDescriptorBinding(std::string passName);
    void* getMappedMemory();
    void UnmapMemory();

    /**
     * @brief Add a new descriptor to pass binding
     */
    VulkanBuffer& addDescriptorSetPerPassBinding(const std::string& passName, uint32_t binding, VkShaderStageFlags shaderStageFlag);
    VulkanBuffer& addDescriptorSetPerPassBindings(const std::vector<PassBindingInfo>&);
    VkDescriptorBufferInfo getVkDescriptorBufferInfo(VkDeviceSize offset = 0);
    uint32_t getPassDescriptorBinding(std::string passName);

  private:
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

    // Holds ´Pass to Descriptor´ bindings
    std::map<std::string, VkDescriptorSetLayoutBinding> m_DescriptorSetBindings;
};

}  // namespace RAYX
#endif