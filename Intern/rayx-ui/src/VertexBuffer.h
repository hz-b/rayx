#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

class VertexBuffer {
  public:
    VertexBuffer(Device& device);
    ~VertexBuffer();

    void createVertexBuffer(const void* data, VkDeviceSize size);

    VkBuffer getVertexBuffer() const { return m_VertexBuffer; }

  private:
    Device& m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
};