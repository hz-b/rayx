#pragma once

#include <vulkan/vulkan.h>

class VertexBuffer {
  public:
    VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
    ~VertexBuffer();

    void createVertexBuffer(const void* data, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkBuffer getVertexBuffer() const { return m_VertexBuffer; }

  private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
};