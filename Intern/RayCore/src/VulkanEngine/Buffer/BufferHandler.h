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
class RAYX_API BufferHandler : protected VulkanEngine {
  public:
    BufferHandler();
    ~BufferHandler() = default;

    template <typename T>
    void createBuffer(VulkanBufferCreateInfo createInfo, const std::vector<T>& vec = nullptr);
    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname);
    template <typename T>
    void updateBuffer(const char* bufname, const std::vector<T>& vec = nullptr);
    void deleteBuffer(const char* bufname);
    void freeBuffer(const char* bufname);
    void waitTransferQueueIdle();

    const VulkanBuffer& getStagingBuffer() const { return m_StagingBuffer; };

  private:
    void createStagingBuffer();
    void createTransferCommandBuffer();
    void createTransferQueue();

    void loadFromStagingBuffer(char* outdata, size_t bytes);
    void storeToStagingBuffer(char* indata, size_t bytes);

    void readBufferRaw(const char* bufname, char* outdata, const VkQueue& computeQueue = nullptr);
    void writeBufferRaw(const char* bufname, char* indata);
    void gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes);
    void gpuMemcpy(const char*, size_t, const char*, size_t, size_t);

    // Used only for transfer
    VkQueue m_TransferQueue;
    VkCommandBuffer m_TransferCommandBuffer;
    VulkanBuffer m_StagingBuffer;
    std::map<std::string, VulkanBuffer> m_Buffers;
};
}  // namespace RAYX
#endif