#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanBuffer.h"

namespace RAYX {
class VulkanEngine;
/**
 * @brief Controls Buffer allocation, creation and IO
 *
 */
class RAYX_API BufferHandler : protected VulkanEngine {
  public:
    BufferHandler();
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

    const VulkanBuffer& getStagingBuffer() const { return m_StagingBuffer; };
    // TODO(OS): This function should be almost illegal...
    const VulkanBuffer& getVulkanBuffer(std::string bufferName) { return m_Buffers[bufferName]; };

    std::vector<VkDescriptorSetLayoutBinding> getVulkanBufferBindings();

  private:
    void createStagingBuffer();
    void createTransferCommandBuffer();
    void createTransferQueue();

    void loadFromStagingBuffer(char* outdata, size_t bytes);
    void storeToStagingBuffer(char* indata, size_t bytes);

    void readBufferRaw(const char* bufname, char* outdata, const VkQueue& computeQueue = nullptr);
    void writeBufferRaw(const char* bufname, char* indata);
    void gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes);

    // Used only for transfer
    VkQueue m_TransferQueue = VK_NULL_HANDLE;
    VkCommandBuffer m_TransferCommandBuffer = VK_NULL_HANDLE;
    VulkanBuffer m_StagingBuffer;
    std::map<std::string, VulkanBuffer> m_Buffers;

    // TODO(OS): Use this instead of m_Buffers once ready
    std::map<std::string, VulkanBuffer> m_ComputeBuffers;
    std::map<std::string, VulkanBuffer> m_GraphicsBuffers;
};
}  // namespace RAYX
#endif