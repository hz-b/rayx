#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanBuffer.h"
#include "VulkanEngine/Init/Fence.h"

namespace RAYX {

class VulkanEngine;

/**
 * @brief Controls Buffer allocation, creation and IO
 *
 */
class RAYX_API BufferHandler {
  public:
    BufferHandler(VkDevice& device, VmaAllocator allocator, uint32_t queueFamilyIndex, size_t stagingSize);
    ~BufferHandler();

    template <typename T>
    void createBuffer(VulkanBufferCreateInfo createInfo, const std::vector<T>& vec = nullptr);

    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname, bool indirect);

    template <typename T>
    void updateBuffer(const char* bufname, const std::vector<T>& vec);

    void deleteBuffer(const char* bufname);
    void freeBuffer(const char* bufname);
    void waitTransferQueueIdle();

    const VulkanBuffer& getStagingBuffer() const { return m_StagingBuffer; }
    // TODO(OS): This function should be almost illegal...
    const VulkanBuffer& getVulkanBuffer(std::string bufferName) { return m_Buffers[bufferName]; }

    const VkFence* getTransferFence() const { return m_TransferFence->fence(); }

    std::vector<VkDescriptorSetLayoutBinding> getVulkanBufferBindings();

  private:
    void createStagingBuffer();
    void createTransferCommandBuffer();
    void createTransferQueue();
    void createTransferFence();

    void loadFromStagingBuffer(char* outdata, size_t bytes);
    void storeToStagingBuffer(char* indata, size_t bytes);

    void readBufferRaw(const char* bufname, char* outdata, const VkQueue& queue = nullptr);
    void writeBufferRaw(const char* bufname, char* indata);
    void gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes);

  private:
    VkDevice& m_Device;
    // Used only for transfer
    VkQueue m_TransferQueue = VK_NULL_HANDLE;
    uint32_t m_FamilyIndex;

    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkCommandBuffer m_TransferCommandBuffer = VK_NULL_HANDLE;

    VmaAllocator m_VmaAllocator;

    VulkanBuffer m_StagingBuffer;
    size_t m_StagingSize;

    std::map<std::string, VulkanBuffer> m_Buffers;

    // TODO(OS): Use this instead of m_Buffers once ready
    std::map<std::string, VulkanBuffer> m_ComputeBuffers;
    std::map<std::string, VulkanBuffer> m_GraphicsBuffers;

    std::unique_ptr<Fence> m_TransferFence;
};
}  // namespace RAYX
#endif