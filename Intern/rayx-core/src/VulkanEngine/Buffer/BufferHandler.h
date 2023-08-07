#ifndef NO_VULKAN

#pragma once

#include <map>
#include <vulkan/vulkan.hpp>

#include "VulkanEngine/Buffer/VulkanBuffer.h"
#include "VulkanEngine/Init/Descriptor.h"
#include "VulkanEngine/Init/Fence.h"

namespace RAYX {

/**
 * @brief Controls Buffer creation, allocation and, IO operations
 *
 */
class RAYX_API BufferHandler {
  public:
    BufferHandler(VkDevice& device, VmaAllocator allocator, uint32_t queueFamilyIndex, size_t stagingSize);
    ~BufferHandler();

    /** @brief Create an empty buffer
     *
     * @param createInfo
     */
    VulkanBuffer& createBuffer(VulkanBufferCreateInfo createInfo);

    /**
     * @brief Create a buffer and fill it with vec
     *
     * @tparam T
     * @param createInfo Buffer creation Info
     * @param vec Vector to fill Buffer with
     */
    template <typename T>
    VulkanBuffer& createBuffer(VulkanBufferCreateInfo createInfo, const std::vector<T>& vec = nullptr);

    template <typename T>
    void updateBuffer(const char* bufname, const std::vector<T>& vec);

    /**
     * @brief Read Buffer
     *
     * @tparam T
     * @param bufname
     * @param indirect wait for transfer queue before read or not
     * @return std::vector<T>
     */
    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname, bool waitForQueue);

    void deleteBuffer(const char* bufname);
    void waitTransferQueueIdle();

    std::map<std::string, std::unique_ptr<VulkanBuffer>>* getBuffers() { return &m_Buffers; }
    const VulkanBuffer& getStagingBuffer() const { return *m_StagingBuffer; }
    inline VulkanBuffer* getBuffer(const std::string& name);
    std::vector<VkDescriptorSetLayoutBinding> getDescriptorBindings(const std::string& passName);
    const VkFence* getTransferFence() const { return m_TransferFence->fence(); }

    void insertBufferMemoryBarrier(const std::string& bufferName, const VkCommandBuffer& commandBuffer, VkAccessFlags srcAccessMask,
                                   VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                   VkDeviceSize offset = 0);

    using Buffer = std::unique_ptr<VulkanBuffer>;

  private:
    void createStagingBuffer();
    void createTransferCommandBuffer();
    void createTransferQueue();
    void createTransferFence();
    void createTransferSemaphore();

    void loadFromStagingBuffer(char* outdata, size_t bytes);
    void storeToStagingBuffer(char* indata, size_t bytes);

    void readBufferRaw(const char* bufname, char* outdata, const VkQueue& queue = nullptr);
    void writeBufferRaw(const char* bufname, char* indata);
    void gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes);

    bool isBufferPresent(const std::string&);

  private:
    VkDevice& m_Device;
    VkQueue m_TransferQueue = VK_NULL_HANDLE;  // Used only for transfer
    uint32_t m_FamilyIndex;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkCommandBuffer m_TransferCommandBuffer = VK_NULL_HANDLE;
    VmaAllocator m_VmaAllocator;
    Buffer m_StagingBuffer;
    size_t m_StagingSize;
    std::unique_ptr<Fence> m_TransferFence;
    VkSemaphore m_TransferSemaphore{};

    std::map<std::string, Buffer> m_Buffers = {};
    // TODO(OS): Use this instead of m_Buffers once ready
    std::map<std::string, Buffer> m_ComputeBuffers;
    std::map<std::string, Buffer> m_GraphicsBuffers;
};
// Template implementations
}  // namespace RAYX
#include "BufferHandler_impl.tpp"
#endif