#pragma once

#include "Device.h"

class Buffer {
  public:
    Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
           VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // Functions for both static and dynamic buffers
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    // Functions for static buffers
    void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    // Functions for dynamic buffers
    void writeToIndex(void* data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

    // Getters
    VkBuffer getBuffer() const { return m_Buffer; }
    void* getMappedMemory() const { return m_mapped; }
    uint32_t getInstanceCount() const { return m_instanceCount; }
    VkDeviceSize getInstanceSize() const { return m_InstanceSize; }
    VkDeviceSize getAlignmentSize() const { return m_InstanceSize; }
    VkBufferUsageFlags getUsageFlags() const { return m_UsageFlags; }
    VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_MemoryPropertyFlags; }
    VkDeviceSize getBufferSize() const { return m_BufferSize; }

  private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    Device& m_Device;
    void* m_mapped = nullptr;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;

    VkDeviceSize m_BufferSize;
    uint32_t m_instanceCount;
    VkDeviceSize m_InstanceSize;
    VkDeviceSize m_AlignmentSize;
    VkBufferUsageFlags m_UsageFlags;
    VkMemoryPropertyFlags m_MemoryPropertyFlags;
};