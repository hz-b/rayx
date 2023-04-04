#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanBuffer.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief Controls Buffer allocation, creation and IO
 *
 */
class RAYX_API BufferManager {
  public:
    BufferManager(const VkQueue&, const VkCommandBuffer&, const std::unique_ptr<VulkanEngine::Fence>&, size_t stagingSize, VmaAllocator&);
    ~BufferManager() = default;

    void readBufferRaw(const char* bufname, char* outdata, const VkQueue& computeQueue = nullptr);
    void writeBufferRaw(const char* bufname, char* indata);
    void gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes);
    void gpuMemcpy(const char*, size_t, const char*, size_t, size_t);
    void waitTransferQueueIdle();

    const VulkanBuffer& getStagingBuffer() const { return m_StagingBuffer; };

  private:
    void loadFromStagingBuffer(char* outdata, size_t bytes);
    void storeToStagingBuffer(char* indata, size_t bytes);
    void createStagingBuffer();
    const VkQueue& m_TransferQueue;
    const VkCommandBuffer& m_TransferCommandBuffer;
    const std::unique_ptr<VulkanEngine::Fence>& m_Fence;
    size_t STAGING_SIZE;
    VmaAllocator& m_VmaAllocator;
    VulkanBuffer m_StagingBuffer;
    std::map<std::string, VulkanBuffer> m_Buffers;
};
}  // namespace RAYX
#endif