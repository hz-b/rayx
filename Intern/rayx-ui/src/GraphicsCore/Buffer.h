#pragma once

#include <string>

#include "Device.h"

/**
 * @class Buffer
 * @brief Manages Vulkan Buffer memory and functionalities.
 *
 * The Buffer class handles the creation, mapping, and manipulation of Vulkan Buffers.
 * This class is intended for use with Device objects and can be used for both static and dynamic buffers.
 */
class Buffer {
  public:
    /**
     * @brief Constructs a new Buffer object.
     * @param device Reference to the Device object.
     * @param name Name of the buffer
     * @param instanceSize Size of each instance in the buffer.
     * @param instanceCount Number of instances.
     * @param usageFlags Usage flags for the Vulkan buffer.
     * @param memoryPropertyFlags Memory property flags.
     * @param minOffsetAlignment Minimum required alignment offset.
     */
    Buffer(const Device& device, const std::string& name, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
           VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
    /// Destroys the Buffer object and frees memory.
    ~Buffer();

    /// Deleted copy constructor
    Buffer(const Buffer&) = delete;
    /// Deleted copy assignment operator
    Buffer& operator=(const Buffer&) = delete;

    /**
     * @brief Maps the buffer into application address space.
     * @param size The number of bytes to map from the starting `offset`.
     *             Use VK_WHOLE_SIZE to map all remaining bytes from the `offset` to the end of the buffer.
     * @param offset The byte offset from which to start mapping.
     *               It is important that this offset be a multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize
     *               when the memory property flags include VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
     * @return Vulkan result of the operation. VK_SUCCESS on successful map operation, error code otherwise.
     */
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /**
     * @brief Unmaps the buffer from the application's address space.
     */
    void unmap();

    // Functions for static buffers
    /**
     * @brief Writes data to the currently mapped buffer.
     * @param data Pointer to the data to write.
     * @param size Size of the data to write, or VK_WHOLE_SIZE to write to the entire buffer.
     * @param offset Offset in the buffer where to start writing.
     * @pre The buffer must be mapped before writing, or an assertion will fail.
     */
    void writeToBuffer(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /**
     * @brief Flushes a specified range of mapped buffer memory to ensure that it becomes visible to the device.
     * @param size Size of the memory range to flush, or VK_WHOLE_SIZE to flush the entire buffer.
     * @param offset Offset in the buffer where the flush starts.
     * @return VkResult Status of the memory flush operation.
     */
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /**
     * @brief Invalidates a specified range of mapped buffer memory to ensure that the host's view of the memory reflects any changes made by the
     * device.
     * @param size Size of the memory range to invalidate, or VK_WHOLE_SIZE to invalidate the entire buffer.
     * @param offset Offset in the buffer where the invalidation starts.
     * @return VkResult Status of the memory invalidation operation.
     */
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /**
     * @brief Creates a VkDescriptorBufferInfo struct for this buffer.
     * @param size Size of the memory range to include in the descriptor, or VK_WHOLE_SIZE for the entire buffer.
     * @param offset Offset in the buffer for the descriptor.
     * @return VkDescriptorBufferInfo The descriptor information for this buffer.
     */
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

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

    const Device& m_Device;
    std::string m_Name;
    void* m_mapped          = nullptr;
    VkBuffer m_Buffer       = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;

    VkDeviceSize m_BufferSize;
    VkDeviceSize m_InstanceSize;
    uint32_t m_instanceCount;
    VkDeviceSize m_AlignmentSize;
    VkBufferUsageFlags m_UsageFlags;
    VkMemoryPropertyFlags m_MemoryPropertyFlags;
};
