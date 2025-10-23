#include "Buffer.h"

#include <cassert>
#include <cstring>

/**
 * Allocates buffer memory using given parameters and associates it with a Device.
 * Also calculates memory alignment.
 */
Buffer::Buffer(const Device& device, const std::string& name, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
               VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
    : m_Device{device},
      m_Name{name},
      m_InstanceSize{instanceSize},
      m_instanceCount{instanceCount},
      m_UsageFlags{usageFlags},
      m_MemoryPropertyFlags{memoryPropertyFlags} {
    m_AlignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    m_BufferSize    = m_AlignmentSize * instanceCount;
    device.createBuffer(m_BufferSize, usageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(m_Device.device(), m_Buffer, nullptr);
    vkFreeMemory(m_Device.device(), m_Memory, nullptr);
}

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) { return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1); }
    return instanceSize;
}

/**
 * This function maps a range of the buffer's memory into the application's address space (making it accessible).
 * The mapped range starts from the given `offset` and extends for `size` bytes.
 */
VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(m_Buffer && m_Memory && "Called map on buffer before create");
    return vkMapMemory(m_Device.device(), m_Memory, offset, size, 0, &m_mapped);
}

/**
 * This function unmaps the previously mapped buffer memory, making it inaccessible from the application's address space.
 * It ensures that the unmapping only happens if the memory was actually mapped (`m_mapped` is not nullptr).
 *
 * Note: After the buffer memory has been unmapped, any pointer to it becomes invalid.
 *       Also, it's important to ensure that the host writes or device reads that were made to this memory
 *       range have completed before calling this function, or else undefined behavior could occur.
 *
 */
void Buffer::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_Device.device(), m_Memory);
        m_mapped = nullptr;
    }
}

void Buffer::writeToBuffer(const void* data, VkDeviceSize size, VkDeviceSize offset) {
    assert(m_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(m_mapped, data, m_BufferSize);
    } else {
        char* memOffset = (char*)m_mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory              = m_Memory;
    mappedRange.offset              = offset;
    mappedRange.size                = size;
    return vkFlushMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory              = m_Memory;
    mappedRange.offset              = offset;
    mappedRange.size                = size;
    return vkInvalidateMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        m_Buffer,
        offset,
        size,
    };
}

void Buffer::writeToIndex(void* data, int index) { writeToBuffer(data, m_InstanceSize, index * m_AlignmentSize); }
VkResult Buffer::flushIndex(int index) { return flush(m_AlignmentSize, index * m_AlignmentSize); }
VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) { return descriptorInfo(m_AlignmentSize, index * m_AlignmentSize); }
VkResult Buffer::invalidateIndex(int index) { return invalidate(m_AlignmentSize, index * m_AlignmentSize); }
